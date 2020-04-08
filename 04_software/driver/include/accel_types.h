/**
 * @file accel_types.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Common type definitions for accelerator driver
 * @version 1.0.0
 * @date 2020-03-30
 */

#ifndef ACCEL_TYPES_H
#define ACCEL_TYPES_H

#include <stdint.h>

/**
 * @brief Operation types supported by the accelerator
 */
typedef enum {
  ACCEL_OP_NONE = 0,
  ACCEL_OP_MATMUL, /**< Matrix multiplication */
  ACCEL_OP_CONV2D, /**< 2D convolution */
} accel_op_type_t;

/**
 * @brief Status codes for accelerator operations
 */
typedef enum {
  ACCEL_STATUS_OK = 0,
  ACCEL_STATUS_ERROR,
  ACCEL_STATUS_INVALID_PARAM,
  ACCEL_STATUS_NO_MEMORY,
  ACCEL_STATUS_TIMEOUT,
  ACCEL_STATUS_BUSY,
  ACCEL_STATUS_NOT_INITIALIZED
} accel_status_t;

/**
 * @brief Memory buffer descriptor
 */
typedef struct {
  void* host_addr;   /**< Host virtual address */
  uint64_t dev_addr; /**< Device physical address */
  uint32_t size;     /**< Buffer size in bytes */
} accel_buffer_t;

/**
 * @brief Operation parameters
 */
typedef struct {
  accel_op_type_t op_type; /**< Operation type */
  accel_buffer_t input;    /**< Input buffer */
  accel_buffer_t output;   /**< Output buffer */
  accel_buffer_t weights;  /**< Weights buffer   */
  uint32_t flags;          /**< Operation flags */
} accel_op_params_t;

#endif /* ACCEL_TYPES_H */