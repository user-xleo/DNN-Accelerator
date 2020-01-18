/**
 * @file tensor.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Tensor class
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace qnn {

/**
 * @brief Generic tensor class for storing n-dimensional arrays
 * @tparam T Data type of tensor elements
 */
template <typename T>
class Tensor {
 public:
  /** @brief Default constructor */
  Tensor() = default;

  /**
   * @brief Constructs tensor with specified shape
   * @param shape Vector of dimensions
   */
  explicit Tensor(const std::vector<int64_t>& shape) { resize(shape); }

  /**
   * @brief Copy constructor
   * @param other Tensor to copy from
   */
  Tensor(const Tensor& other) : data_(other.data_), shape_(other.shape_) {}

  /**
   * @brief Move constructor
   * @param other Tensor to move from
   */
  Tensor(Tensor&& other) noexcept
      : data_(std::move(other.data_)), shape_(std::move(other.shape_)) {}

  /**
   * @brief Assignment operator
   * @param other Tensor to assign from
   * @return Reference to this tensor
   */
  Tensor& operator=(const Tensor& other) {
    if (this != &other) {
      data_ = other.data_;
      shape_ = other.shape_;
    }
    return *this;
  }

  /**
   * @brief Move assignment operator
   * @param other Tensor to assign from
   * @return Reference to this tensor
   */
  Tensor& operator=(Tensor&& other) noexcept {
    if (this != &other) {
      data_ = std::move(other.data_);
      shape_ = std::move(other.shape_);
      scale_ = other.scale_;
    }
    return *this;
  }

  /**
   * @brief Resize with int64_t vector
   * @param shape Vector of dimensions
   */
  void resize(const std::vector<int64_t>& shape) {
    shape_.clear();
    shape_.reserve(shape.size());
    for (const auto& dim : shape) {
      if (dim < 0) {
        throw std::invalid_argument("Negative dimension size");
      }
      shape_.push_back(static_cast<size_t>(dim));
    }
    size_t total = 1;
    for (const auto& dim : shape_) {
      total *= dim;
    }
    data_.resize(total);
  }

  /**
   * @brief Resize with size_t vector
   * @param shape Vector of dimensions
   */
  void resize(const std::vector<size_t>& shape) {
    shape_ = shape;
    size_t total = 1;
    for (const auto& dim : shape_) {
      total *= dim;
    }
    data_.resize(total);
  }

  /** @return Size of tensor */
  size_t size() const {
    size_t total = 1;
    for (const auto& dim : shape_) {
      total *= dim;
    }
    return total;
  }

  /** @return Reference to tensor shape */
  const std::vector<size_t>& shape() const { return shape_; }

  /** @return Pointer to raw data */
  T* data() { return data_.data(); }

  /** @return Const pointer to raw data */
  const T* data() const { return data_.data(); }

  /**
   * @brief Access tensor element
   * @param index Index of element
   * @return Reference to element
   */
  T& operator[](size_t index) {
    if (index >= data_.size()) {
      throw std::out_of_range("Tensor index out of range");
    }
    return data_[index];
  }

  /**
   * @brief Access tensor element
   * @param index Index of element
   * @return Const reference to element
   */
  const T& operator[](size_t index) const {
    if (index >= data_.size()) {
      throw std::out_of_range("Tensor index out of range");
    }
    return data_[index];
  }

  /**
   * @brief Get tensor scale (for quantized tensors)
   * @return Scale value
   */
  float scale() const { return scale_; }

  /**
   * @brief Set tensor scale (for quantized tensors)
   * @param scale Scale value
   */
  void set_scale(float scale) { scale_ = scale; }

 private:
  std::vector<T> data_;
  std::vector<size_t> shape_;
  float scale_ = 1.0f;
};

/** @brief Specialized tensor types */
using QTensor = Tensor<int8_t>;
using FTensor = Tensor<float>;

}  // namespace qnn