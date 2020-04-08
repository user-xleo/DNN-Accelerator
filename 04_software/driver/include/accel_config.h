/**
 * @file accel_config.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Configuration interface for accelerator driver
 * @version 1.0.0
 * @date 2020-03-30
 */

#ifndef ACCEL_CONFIG_H
#define ACCEL_CONFIG_H

#include "accel_types.h"

/**
 * @brief Configuration flags
 */
#define ACCEL_CONFIG_ENABLE_DMA (1 << 0)
#define ACCEL_CONFIG_SYNC_MODE (1 << 1)
#define ACCEL_CONFIG_HIGH_PRIORITY (1 << 2)

/**
 * @brief Device configuration structure
 */
typedef struct {
  uint32_t flags;        /**< Configuration flags */
  uint32_t num_channels; /**< Number of DMA channels */
  uint32_t max_transfer; /**< Maximum transfer size */
  uint32_t timeout_ms;   /**< Operation timeout in milliseconds */
} accel_config_t;

/**
 * @brief Configure the accelerator device
 * @param config Pointer to configuration structure
 * @return Status code
 */
accel_status_t accel_configure(const accel_config_t* config);

/**
 * @brief Get current device configuration
 * @param config Pointer to store configuration
 * @return Status code
 */
accel_status_t accel_get_config(accel_config_t* config);

/**
 * @brief Reset device configuration to defaults
 * @return Status code
 */
accel_status_t accel_reset_config(void);

#endif /* ACCEL_CONFIG_H */