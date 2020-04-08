/**
 * @file accel_config.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Implementation of configuration interface for accelerator driver
 * @version 1.0.0
 * @date 2020-03-30
 */

#include "accel_config.h"

#include <stdbool.h>
#include <string.h>

#include "hal.h"

// Reference to global context from accel.c
extern struct {
  hal_context_t* hal;
  accel_config_t config;
  char last_error[256];
  bool initialized;
} g_ctx;

accel_status_t accel_configure(const accel_config_t* config) {
  if (!g_ctx.initialized) {
    return ACCEL_STATUS_NOT_INITIALIZED;
  }

  if (!config) {
    return ACCEL_STATUS_INVALID_PARAM;
  }

  // Store configuration
  memcpy(&g_ctx.config, config, sizeof(accel_config_t));

  return ACCEL_STATUS_OK;
}

accel_status_t accel_get_config(accel_config_t* config) {
  if (!g_ctx.initialized) {
    return ACCEL_STATUS_NOT_INITIALIZED;
  }

  if (!config) {
    return ACCEL_STATUS_INVALID_PARAM;
  }

  // Return current configuration
  memcpy(config, &g_ctx.config, sizeof(accel_config_t));

  return ACCEL_STATUS_OK;
}

accel_status_t accel_reset_config(void) {
  if (!g_ctx.initialized) {
    return ACCEL_STATUS_NOT_INITIALIZED;
  }

  // Reset to default configuration
  memset(&g_ctx.config, 0, sizeof(accel_config_t));

  // Set default values
  g_ctx.config.flags = ACCEL_CONFIG_ENABLE_DMA;
  g_ctx.config.num_channels = 1;
  g_ctx.config.max_transfer = 0x1000000;  // 16MB
  g_ctx.config.timeout_ms = 1000;         // 1 second

  return ACCEL_STATUS_OK;
}