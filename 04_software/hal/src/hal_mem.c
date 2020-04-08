/**
 * @file hal_mem.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of memory management for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_mem.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Memory block structure for tracking allocations
 */
struct mem_block {
  void* addr;             /**< Virtual address of block */
  size_t size;            /**< Size of block */
  bool used;              /**< Whether block is in use */
  struct mem_block* next; /**< Next block in list */
};

/**
 * @brief Memory management context
 */
struct hal_mem_context {
  void* base_addr;          /**< Base address of memory region */
  size_t total_size;        /**< Total size of memory region */
  struct mem_block* blocks; /**< List of memory blocks */
};

/**
 * @brief Round up size to alignment boundary
 * @param size Size to align
 * @return Aligned size
 */
static size_t align_size(size_t size) {
  return (size + HAL_MEM_ALIGN - 1) & ~(HAL_MEM_ALIGN - 1);
}

bool hal_mem_init(hal_context_t* ctx, void* base, size_t size) {
  struct hal_mem_context* mem_ctx = malloc(sizeof(struct hal_mem_context));
  if (!mem_ctx) {
    return false;
  }

  struct mem_block* block = malloc(sizeof(struct mem_block));
  if (!block) {
    free(mem_ctx);
    return false;
  }

  block->addr = base;
  block->size = size;
  block->used = false;
  block->next = NULL;

  mem_ctx->base_addr = base;
  mem_ctx->total_size = size;
  mem_ctx->blocks = block;

  ctx->mem_ctx = mem_ctx;
  return true;
}

void hal_mem_cleanup(hal_context_t* ctx) {
  if (!ctx || !ctx->mem_ctx) {
    return;
  }

  struct hal_mem_context* mem_ctx = ctx->mem_ctx;
  struct mem_block* block = mem_ctx->blocks;

  while (block) {
    struct mem_block* next = block->next;
    free(block);
    block = next;
  }

  free(mem_ctx);
  ctx->mem_ctx = NULL;
}

void* hal_mem_alloc(hal_context_t* ctx, size_t size) {
  if (!ctx || !ctx->mem_ctx || size == 0) {
    return NULL;
  }

  struct hal_mem_context* mem_ctx = ctx->mem_ctx;
  size = align_size(size);

  struct mem_block* block = mem_ctx->blocks;
  struct mem_block* best_fit = NULL;
  size_t best_size = (size_t)-1;

  // Find best fit block
  while (block) {
    if (!block->used && block->size >= size) {
      if (block->size < best_size) {
        best_fit = block;
        best_size = block->size;
      }
    }
    block = block->next;
  }

  if (!best_fit) {
    return NULL;
  }

  // Split block if it's significantly larger
  if (best_fit->size > size + sizeof(struct mem_block) + HAL_MEM_ALIGN) {
    struct mem_block* new_block = malloc(sizeof(struct mem_block));
    if (!new_block) {
      return NULL;
    }

    new_block->addr = (char*)best_fit->addr + size;
    new_block->size = best_fit->size - size;
    new_block->used = false;
    new_block->next = best_fit->next;

    best_fit->size = size;
    best_fit->next = new_block;
  }

  best_fit->used = true;
  return best_fit->addr;
}

void hal_mem_free(hal_context_t* ctx, void* ptr) {
  if (!ctx || !ctx->mem_ctx || !ptr) {
    return;
  }

  struct hal_mem_context* mem_ctx = ctx->mem_ctx;
  struct mem_block* block = mem_ctx->blocks;
  struct mem_block* prev = NULL;

  // Find the block
  while (block && block->addr != ptr) {
    prev = block;
    block = block->next;
  }

  if (!block || !block->used) {
    return;
  }

  block->used = false;

  // Merge with next block if it's free
  while (block->next && !block->next->used) {
    struct mem_block* next = block->next;
    block->size += next->size;
    block->next = next->next;
    free(next);
  }

  // Merge with previous block if it's free
  if (prev && !prev->used) {
    prev->size += block->size;
    prev->next = block->next;
    free(block);
  }
}

uint64_t hal_virt_to_phys(hal_context_t* ctx, void* vaddr) {
  if (!ctx || !ctx->mem_ctx) {
    return 0;
  }

  struct hal_mem_context* mem_ctx = ctx->mem_ctx;
  if (vaddr < mem_ctx->base_addr ||
      vaddr >= (mem_ctx->base_addr + mem_ctx->total_size)) {
    return 0;
  }

  return HAL_ACCEL_MEM_BASE + ((uint8_t*)vaddr - (uint8_t*)mem_ctx->base_addr);
}

size_t hal_mem_available(hal_context_t* ctx) {
  if (!ctx || !ctx->mem_ctx) {
    return 0;
  }

  struct hal_mem_context* mem_ctx = ctx->mem_ctx;
  size_t available = 0;
  struct mem_block* block = mem_ctx->blocks;

  while (block) {
    if (!block->used) {
      available += block->size;
    }
    block = block->next;
  }

  return available;
}