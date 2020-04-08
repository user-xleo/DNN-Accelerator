/**
 * @file hal_mem.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Memory management interface for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#ifndef HAL_MEM_H
#define HAL_MEM_H

#include <stddef.h>
#include <stdint.h>

#include "hal_base.h"

// Memory alignment requirement
#define HAL_MEM_ALIGN 64  // 64-byte alignment

/**
 * @brief Initialize memory management subsystem
 * @param ctx HAL context
 * @param base Base address of memory region
 * @param size Size of memory region
 * @return true if successful, false on error
 */
bool hal_mem_init(hal_context_t* ctx, void* base, size_t size);

/**
 * @brief Clean up memory management subsystem
 * @param ctx HAL context
 */
void hal_mem_cleanup(hal_context_t* ctx);

/**
 * @brief Allocate memory from accelerator memory region
 * @param ctx HAL context
 * @param size Size of memory to allocate
 * @return Virtual address of allocated memory, or NULL on failure
 */
void* hal_mem_alloc(hal_context_t* ctx, size_t size);

/**
 * @brief Free previously allocated accelerator memory
 * @param ctx HAL context
 * @param ptr Virtual address to free
 */
void hal_mem_free(hal_context_t* ctx, void* ptr);

/**
 * @brief Convert virtual address to physical address
 * @param ctx HAL context
 * @param vaddr Virtual address to convert
 * @return Physical address, or 0 on error
 */
uint64_t hal_virt_to_phys(hal_context_t* ctx, void* vaddr);

/**
 * @brief Get available memory size
 * @param ctx HAL context
 * @return Total size of free memory blocks
 */
size_t hal_mem_available(hal_context_t* ctx);

#endif /* HAL_MEM_H */