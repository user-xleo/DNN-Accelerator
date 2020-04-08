/**
 * @file hal_io.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of I/O operations for hardware accelerator
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_io.h"

#include <unistd.h>

bool hal_wait_for_ready(hal_context_t* ctx) {
  if (!ctx) {
    return false;
  }

  // Simple polling implementation
  int retries = 100;  // Maximum retries
  while (retries-- > 0) {
    if (hal_is_ready(ctx)) {
      return true;
    }
    usleep(1000);  // Wait 1ms between checks
  }
  return false;
}

bool hal_is_ready(hal_context_t* ctx) {
  if (!ctx) {
    return false;
  }
  return (ctx->status & HAL_STATUS_READY) != 0;
}

bool hal_is_busy(hal_context_t* ctx) {
  if (!ctx) {
    return false;
  }
  return (ctx->status & HAL_STATUS_BUSY) != 0;
}

bool hal_is_error(hal_context_t* ctx) {
  if (!ctx) {
    return false;
  }
  return (ctx->status & HAL_STATUS_ERROR) != 0;
}

uint32_t hal_get_status(hal_context_t* ctx) {
  if (!ctx) {
    return 0;
  }
  return ctx->status;
}

void hal_set_status(hal_context_t* ctx, uint32_t status) {
  if (ctx) {
    ctx->status = status;
  }
}
