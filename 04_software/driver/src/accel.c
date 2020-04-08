/**
 * @file accel.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of main accelerator driver interface
 * @version 1.0.0
 * @date 2020-03-30
 */

#include "accel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal.h"
#include "hal_config.h"
#include "hal_io.h"
#include "hal_mem.h"

// Driver context
struct {
  hal_context_t* hal;
  accel_config_t config;
  char last_error[256];
  bool initialized;
} g_ctx = {0};

// HAL operation codes mapping
#define HAL_OP_MATMUL 0x01
#define HAL_OP_CONV 0x02

// Convert HAL status to driver status
static accel_status_t convert_hal_status(uint32_t hal_status) {
  if (hal_status & HAL_STATUS_ERROR) {
    return ACCEL_STATUS_ERROR;
  }
  if (hal_status & HAL_STATUS_BUSY) {
    return ACCEL_STATUS_BUSY;
  }
  return ACCEL_STATUS_OK;
}

accel_status_t accel_init(const char* device_path) {
  if (!device_path) {
    return ACCEL_STATUS_INVALID_PARAM;
  }

  if (g_ctx.initialized) {
    return ACCEL_STATUS_OK;
  }

  // Initialize HAL
  g_ctx.hal = hal_init(device_path);
  if (!g_ctx.hal) {
    snprintf(g_ctx.last_error, sizeof(g_ctx.last_error),
             "Failed to initialize HAL");
    return ACCEL_STATUS_ERROR;
  }

  g_ctx.initialized = true;
  return ACCEL_STATUS_OK;
}

void accel_cleanup(void) {
  if (g_ctx.initialized) {
    hal_cleanup(g_ctx.hal);
    memset(&g_ctx, 0, sizeof(g_ctx));
  }
}

accel_buffer_t* accel_alloc_buffer(uint32_t size) {
  if (!g_ctx.initialized) {
    return NULL;
  }

  accel_buffer_t* buffer = malloc(sizeof(accel_buffer_t));
  if (!buffer) {
    snprintf(g_ctx.last_error, sizeof(g_ctx.last_error),
             "Failed to allocate buffer descriptor");
    return NULL;
  }

  buffer->host_addr = hal_mem_alloc(g_ctx.hal, size);
  if (!buffer->host_addr) {
    free(buffer);
    snprintf(g_ctx.last_error, sizeof(g_ctx.last_error),
             "Failed to allocate device memory");
    return NULL;
  }

  buffer->dev_addr = hal_virt_to_phys(g_ctx.hal, buffer->host_addr);
  buffer->size = size;

  return buffer;
}

void accel_free_buffer(accel_buffer_t* buffer) {
  if (buffer && g_ctx.initialized) {
    if (buffer->host_addr) {
      hal_mem_free(g_ctx.hal, buffer->host_addr);
    }
    free(buffer);
  }
}

accel_status_t accel_submit_op(const accel_op_params_t* params) {
  if (!g_ctx.initialized) {
    return ACCEL_STATUS_NOT_INITIALIZED;
  }

  if (!params) {
    return ACCEL_STATUS_INVALID_PARAM;
  }

  hal_systolic_config_t systolic_cfg = {0};
  hal_lsu_config_t lsu_cfg = {0};

  switch (params->op_type) {
    case ACCEL_OP_MATMUL:
      systolic_cfg.opcode = HAL_OP_MATMUL;
      systolic_cfg.control = params->flags;
      if (!hal_configure_systolic(g_ctx.hal, &systolic_cfg)) {
        return ACCEL_STATUS_ERROR;
      }
      break;

    case ACCEL_OP_CONV2D:
      systolic_cfg.opcode = HAL_OP_CONV;
      systolic_cfg.control = params->flags;
      if (!hal_configure_systolic(g_ctx.hal, &systolic_cfg)) {
        return ACCEL_STATUS_ERROR;
      }
      break;

    default:
      return ACCEL_STATUS_INVALID_PARAM;
  }

  // Configure LSU for data transfer
  lsu_cfg.src_addr = params->input.dev_addr;
  lsu_cfg.dst_addr = params->output.dev_addr;
  lsu_cfg.length = params->input.size;
  if (!hal_configure_lsu(g_ctx.hal, &lsu_cfg)) {
    return ACCEL_STATUS_ERROR;
  }

  return ACCEL_STATUS_OK;
}

accel_status_t accel_wait_complete(uint32_t timeout_ms) {
  if (!g_ctx.initialized) {
    return ACCEL_STATUS_NOT_INITIALIZED;
  }

  if (!hal_wait_for_ready(g_ctx.hal)) {
    snprintf(g_ctx.last_error, sizeof(g_ctx.last_error), "Operation timed out");
    return ACCEL_STATUS_TIMEOUT;
  }

  return convert_hal_status(hal_get_status(g_ctx.hal));
}

const char* accel_get_error(void) { return g_ctx.last_error; }