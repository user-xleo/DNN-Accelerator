/**
 * @file hal_base.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Hardware abstraction layer base definitions
 * @version 1.0.0
 * @date 2020-03-28
 */

#ifndef HAL_BASE_H
#define HAL_BASE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Status definitions
#define HAL_STATUS_READY 0x1
#define HAL_STATUS_BUSY 0x2
#define HAL_STATUS_COMPLETE 0x4
#define HAL_STATUS_ERROR 0x8

// Memory region definitions
#define HAL_ACCEL_MEM_BASE 0x30000000
#define HAL_ACCEL_MEM_SIZE (256 * 1024 * 1024)  // 256MB

/**
 * @brief HAL context structure
 */
struct hal_context {
  int fd;                   /**< File descriptor for device */
  uint32_t status;          /**< Current hardware status */
  void* mapped_memory;      /**< Memory mapped region for registers */
  void* accel_memory_base;  /**< Base of mapped accelerator memory */
  size_t accel_memory_size; /**< Size of mapped accelerator memory */
  void* mem_ctx;            /**< Memory management context */
};

typedef struct hal_context hal_context_t;

// Base HAL operations
hal_context_t* hal_init(const char* device_path);
void hal_cleanup(hal_context_t* ctx);

#endif /* HAL_BASE_H */