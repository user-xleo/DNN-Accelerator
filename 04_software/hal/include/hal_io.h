/**
 * @file hal_io.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief I/O operations interface for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#ifndef HAL_IO_H
#define HAL_IO_H

#include <stdbool.h>
#include <stdint.h>

#include "hal_base.h"

/**
 * @brief Wait for hardware to be ready
 * @param ctx HAL context
 * @return true if ready, false on timeout or error
 */
bool hal_wait_for_ready(hal_context_t* ctx);

/**
 * @brief Check if hardware is ready
 * @param ctx HAL context
 * @return true if ready, false otherwise
 */
bool hal_is_ready(hal_context_t* ctx);

/**
 * @brief Check if hardware is busy
 * @param ctx HAL context
 * @return true if busy, false otherwise
 */
bool hal_is_busy(hal_context_t* ctx);

/**
 * @brief Check if hardware is in error state
 * @param ctx HAL context
 * @return true if error, false otherwise
 */
bool hal_is_error(hal_context_t* ctx);

/**
 * @brief Get current hardware status
 * @param ctx HAL context
 * @return Current status value
 */
uint32_t hal_get_status(hal_context_t* ctx);

/**
 * @brief Set hardware status
 * @param ctx HAL context
 * @param status New status value
 */
void hal_set_status(hal_context_t* ctx, uint32_t status);

#endif /* HAL_IO_H */