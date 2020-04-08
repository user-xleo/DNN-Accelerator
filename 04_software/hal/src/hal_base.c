/**
 * @file hal_base.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of base HAL functionality for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_base.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "hal_mem.h"

hal_context_t* hal_init(const char* device_path) {
  hal_context_t* ctx = (hal_context_t*)malloc(sizeof(hal_context_t));
  if (!ctx) {
    return NULL;
  }

  // Open device file
  ctx->fd = open(device_path, O_RDWR);
  if (ctx->fd < 0) {
    free(ctx);
    return NULL;
  }

  // Map register space
  ctx->mapped_memory =
      mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fd, 0);
  if (ctx->mapped_memory == MAP_FAILED) {
    close(ctx->fd);
    free(ctx);
    return NULL;
  }

  // Map accelerator memory region
  ctx->accel_memory_base =
      mmap(NULL, HAL_ACCEL_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
           ctx->fd, HAL_ACCEL_MEM_BASE);
  if (ctx->accel_memory_base == MAP_FAILED) {
    munmap(ctx->mapped_memory, getpagesize());
    close(ctx->fd);
    free(ctx);
    return NULL;
  }

  ctx->accel_memory_size = HAL_ACCEL_MEM_SIZE;
  ctx->status = HAL_STATUS_READY;

  // Initialize memory management
  if (!hal_mem_init(ctx, ctx->accel_memory_base, ctx->accel_memory_size)) {
    munmap(ctx->accel_memory_base, ctx->accel_memory_size);
    munmap(ctx->mapped_memory, getpagesize());
    close(ctx->fd);
    free(ctx);
    return NULL;
  }

  return ctx;
}

void hal_cleanup(hal_context_t* ctx) {
  if (ctx) {
    hal_mem_cleanup(ctx);
    if (ctx->mapped_memory) {
      munmap(ctx->mapped_memory, getpagesize());
    }
    if (ctx->accel_memory_base) {
      munmap(ctx->accel_memory_base, ctx->accel_memory_size);
    }
    if (ctx->fd >= 0) {
      close(ctx->fd);
    }
    free(ctx);
  }
}
