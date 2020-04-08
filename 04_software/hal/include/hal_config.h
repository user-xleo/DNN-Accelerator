/**
 * @file hal_config.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Hardware configuration structures and functions
 * @version 1.0.0
 * @date 2020-03-28
 */

#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include <stdint.h>

#include "hal_base.h"

/**
 * @brief LSU (Load-Store Unit) configuration structure
 */
typedef struct __attribute__((packed)) {
  uint32_t opcode;   /**< Operation code */
  uint64_t src_addr; /**< Source memory address */
  uint64_t dst_addr; /**< Destination memory address */
  uint32_t length;   /**< Data transfer length */
  uint32_t control;  /**< Control flags */
  uint32_t status;   /**< Operation status */
} hal_lsu_config_t;

/**
 * @brief Systolic array configuration structure
 */
typedef struct __attribute__((packed)) {
  uint32_t opcode;       /**< Operation code (conv, matmul, etc.) */
  uint32_t in_height;    /**< Input height */
  uint32_t in_width;     /**< Input width */
  uint32_t in_channels;  /**< Input channels */
  uint32_t out_height;   /**< Output height */
  uint32_t out_width;    /**< Output width */
  uint32_t out_channels; /**< Output channels */
  uint32_t stride;       /**< Stride value */
  uint32_t control;      /**< Control flags (ReLU, quantization, etc.) */
  uint32_t status;       /**< Operation status */
} hal_systolic_config_t;

/**
 * @brief IMG2COL configuration structure
 */
typedef struct __attribute__((packed)) {
  uint32_t opcode;      /**< Operation code */
  uint32_t in_height;   /**< Input image height */
  uint32_t in_width;    /**< Input image width */
  uint32_t in_channels; /**< Input image channels */
  uint32_t kernel_size; /**< Convolution kernel size */
  uint32_t stride;      /**< Stride value */
  uint32_t pad;         /**< Padding size */
  uint32_t control;     /**< Control flags */
  uint32_t status;      /**< Operation status */
} hal_img2col_config_t;

/**
 * @brief Controller instruction register structure
 *
 * This structure represents the hardware control registers. The actual address
 * mapping is handled by the device driver through /dev/accelerator.
 */
typedef struct __attribute__((packed)) {
  uint32_t opcode;   /**< Operation code */
  uint64_t src_addr; /**< Source address */
  uint64_t dst_addr; /**< Destination address */
  uint32_t length;   /**< Data length */
  uint32_t control;  /**< Control signals */
  uint32_t status;   /**< Operation status */

  union {
    hal_lsu_config_t lsu;                 /**< LSU operation */
    hal_systolic_config_t systolic_array; /**< Convolution operation */
    hal_img2col_config_t img2col;         /**< Image conversion operation */
  } ir_data;
} hal_controller_ir_t;

/**
 * @brief Configure the LSU unit
 * @param ctx HAL context
 * @param config LSU configuration
 * @return true if successful, false on error
 */
bool hal_configure_lsu(hal_context_t* ctx, const hal_lsu_config_t* config);

/**
 * @brief Configure the systolic array
 * @param ctx HAL context
 * @param config Systolic array configuration
 * @return true if successful, false on error
 */
bool hal_configure_systolic(hal_context_t* ctx,
                            const hal_systolic_config_t* config);

/**
 * @brief Configure the IMG2COL unit
 * @param ctx HAL context
 * @param config IMG2COL configuration
 * @return true if successful, false on error
 */
bool hal_configure_img2col(hal_context_t* ctx,
                           const hal_img2col_config_t* config);

#endif /* HAL_CONFIG_H */