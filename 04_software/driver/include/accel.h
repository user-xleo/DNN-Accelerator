/**
 * @file accel.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Main interface for accelerator driver
 * @version 1.0.0
 * @date 2020-03-30
 */

#ifndef ACCEL_H
#define ACCEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "accel_config.h"
#include "accel_types.h"
/**
 * @brief Initialize the accelerator
 * @param device_path Path to device file
 * @return Status code
 */
accel_status_t accel_init(const char* device_path);

/**
 * @brief Clean up accelerator resources
 */
void accel_cleanup(void);

/**
 * @brief Allocate memory buffer for accelerator operations
 * @param size Buffer size in bytes
 * @return Buffer descriptor, NULL on failure
 */
accel_buffer_t* accel_alloc_buffer(uint32_t size);

/**
 * @brief Free allocated memory buffer
 * @param buffer Buffer to free
 */
void accel_free_buffer(accel_buffer_t* buffer);

/**
 * @brief Submit operation to accelerator
 * @param params Operation parameters
 * @return Status code
 */
accel_status_t accel_submit_op(const accel_op_params_t* params);

/**
 * @brief Wait for operation completion
 * @param timeout_ms Timeout in milliseconds (0 for infinite)
 * @return Status code
 */
accel_status_t accel_wait_complete(uint32_t timeout_ms);

/**
 * @brief Get last error message
 * @return Error message string
 */
const char* accel_get_error(void);
#ifdef __cplusplus
}
#endif
#endif /* ACCEL_H */