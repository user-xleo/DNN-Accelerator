/**
 * @file runtime.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Runtime interface for hardware accelerator
 * @version 1.0.0
 * @date 2020-04-08
 */

#pragma once

#include <string>

#include "buffer.hpp"
#include "types.hpp"

namespace accel {

/**
 * @brief Runtime interface for accelerator operations
 */
class Runtime {
 public:
  /**
   * @brief Initialize runtime with device path
   * @param device_path Path to accelerator device
   * @throws std::runtime_error if initialization fails
   */
  explicit Runtime(const std::string& device_path) {
    if (accel_init(device_path.c_str()) != ACCEL_STATUS_OK) {
      throw std::runtime_error("Failed to initialize runtime: " +
                               std::string(accel_get_error()));
    }

    // Set default configuration
    if (accel_reset_config() != ACCEL_STATUS_OK) {
      throw std::runtime_error("Failed to reset configuration: " +
                               std::string(accel_get_error()));
    }
  }

  ~Runtime() { accel_cleanup(); }

  // Disable copying
  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;

  /**
   * @brief Configure runtime parameters
   * @param flags Configuration flags
   * @param num_channels Number of DMA channels
   * @param max_transfer Maximum transfer size
   * @param timeout_ms Operation timeout in milliseconds
   * @throws std::runtime_error if configuration fails
   */
  void Configure(uint32_t flags, uint32_t num_channels = 1,
                 uint32_t max_transfer = 0x1000000,
                 uint32_t timeout_ms = 1000) {
    accel_config_t config{};
    config.flags = flags;
    config.num_channels = num_channels;
    config.max_transfer = max_transfer;
    config.timeout_ms = timeout_ms;

    if (accel_configure(&config) != ACCEL_STATUS_OK) {
      throw std::runtime_error("Failed to configure runtime: " +
                               std::string(accel_get_error()));
    }
  }

  /**
   * @brief Execute matrix multiplication
   * @param input Input buffer
   * @param weights Weight buffer
   * @param output Output buffer
   * @throws std::runtime_error if operation fails
   */
  void MatrixMultiply(const Buffer& input, const Buffer& weights,
                      Buffer& output) {
    accel_op_params_t params{};
    params.op_type = ACCEL_OP_MATMUL;
    params.input = *input.buffer_;
    params.weights = *weights.buffer_;
    params.output = *output.buffer_;

    SubmitAndWait(params);
  }

  /**
   * @brief Execute 2D convolution
   * @param input Input buffer
   * @param weights Weight buffer
   * @param output Output buffer
   * @throws std::runtime_error if operation fails
   */
  void Convolution2D(const Buffer& input, const Buffer& weights,
                     Buffer& output) {
    accel_op_params_t params{};
    params.op_type = ACCEL_OP_CONV2D;
    params.input = *input.buffer_;
    params.weights = *weights.buffer_;
    params.output = *output.buffer_;

    SubmitAndWait(params);
  }

 private:
  /**
   * @brief Submit operation and wait for completion
   * @param params Operation parameters
   * @throws std::runtime_error if operation fails
   */
  void SubmitAndWait(const accel_op_params_t& params) {
    accel_status_t status = accel_submit_op(&params);
    if (status != ACCEL_STATUS_OK) {
      throw std::runtime_error("Failed to submit operation: " +
                               std::string(accel_get_error()));
    }

    status = accel_wait_complete(0);  // Wait indefinitely
    if (status != ACCEL_STATUS_OK) {
      throw std::runtime_error("Operation failed: " +
                               std::string(accel_get_error()));
    }
  }
};

}  // namespace accel