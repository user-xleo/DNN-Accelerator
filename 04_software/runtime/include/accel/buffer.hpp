/**
 * @file buffer.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief RAII wrapper for accelerator buffer
 * @version 1.0.0
 * @date 2020-04-08
 */

#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "accel.h"

namespace accel {

/**
 * @brief RAII wrapper for accelerator buffer
 */
class Buffer {
 public:
  /**
   * @brief Creates a buffer of specified size
   * @param size Buffer size in bytes
   * @throws std::runtime_error if allocation fails
   */
  explicit Buffer(size_t size) {
    buffer_ = accel_alloc_buffer(size);
    if (!buffer_) {
      throw std::runtime_error("Failed to allocate buffer: " +
                               std::string(accel_get_error()));
    }
  }

  ~Buffer() {
    if (buffer_) {
      accel_free_buffer(buffer_);
    }
  }

  // Disable copying
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  // Enable moving
  Buffer(Buffer&& other) noexcept : buffer_(other.buffer_) {
    other.buffer_ = nullptr;
  }

  Buffer& operator=(Buffer&& other) noexcept {
    if (this != &other) {
      if (buffer_) {
        accel_free_buffer(buffer_);
      }
      buffer_ = other.buffer_;
      other.buffer_ = nullptr;
    }
    return *this;
  }

  /**
   * @brief Get raw pointer to host memory
   * @return Pointer to host memory
   */
  void* data() const { return buffer_->host_addr; }

  /**
   * @brief Get buffer size
   * @return Size in bytes
   */
  size_t size() const { return buffer_->size; }

 private:
  accel_buffer_t* buffer_;
  friend class Runtime;
};

}  // namespace accel