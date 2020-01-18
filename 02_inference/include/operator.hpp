/**
 * @file operator.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Operator class
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once

#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "tensor.hpp"

namespace qnn {

using json = nlohmann::json;

/**
 * @brief Weight information for quantized operators
 *
 * This class holds quantization parameters and weight values for neural network
 * operators. It supports both per-tensor and per-channel quantization schemes.
 */
class WeightInfo {
 public:
  /**
   * @brief Constructs WeightInfo from JSON data
   *
   * @param j JSON object containing weight parameters
   * @return WeightInfo instance initialized with the parameters
   * @throws json::exception If required parameters are missing
   * @throws std::runtime_error If dtype is unsupported
   */
  template <typename T = int8_t>
  static WeightInfo LoadFromJson(const json& j) {
    WeightInfo info;

    // Parse shape
    info.shape_ = j["shape"].get<std::vector<int64_t>>();

    // Parse dtype and validate
    std::string dtype = j["dtype"].get<std::string>();
    if (dtype == "torch.qint8" && !std::is_same_v<T, int8_t>) {
      throw std::runtime_error(
          "Type mismatch: JSON specifies qint8 but template parameter is "
          "different");
    } else if (dtype == "torch.float32" && !std::is_same_v<T, float>) {
      throw std::runtime_error(
          "Type mismatch: JSON specifies float32 but template parameter is "
          "different");
    }

    // Parse quantization type
    if (j.contains("quantization")) {
      info.quantization_ = j["quantization"].get<std::string>();
    }

    // Parse values based on dtype
    if (j.contains("values")) {
      // Calculate total size from shape
      size_t total_size = 1;
      for (auto dim : info.shape_) {
        total_size *= dim;
      }

      // Create tensor with the right size
      info.values_ = QTensor(info.shape_);
      auto* data = info.values_.data();

      if (dtype == "torch.qint8") {
        // Flatten nested arrays recursively
        std::function<void(const json&, int8_t*&)> flatten_array =
            [&flatten_array](const json& arr, int8_t*& out) {
              if (arr.is_array()) {
                for (const auto& elem : arr) {
                  flatten_array(elem, out);
                }
              } else {
                *out++ = arr.get<int8_t>();
              }
            };

        int8_t* ptr = data;
        flatten_array(j["values"], ptr);

      } else if (dtype == "torch.float32") {
        throw std::runtime_error("Float tensor support not implemented yet");
      } else {
        throw std::runtime_error("Unsupported dtype: " + dtype);
      }
    }

    // Parse per-tensor quantization parameters
    if (j.contains("scale")) {
      info.scale_ = j["scale"].get<float>();
    }

    // Parse per-channel quantization parameters
    if (j.contains("scales")) {
      info.scales_ = j["scales"].get<std::vector<float>>();
    }

    if (j.contains("axis")) {
      info.axis_ = j["axis"].get<int>();
    }

    return info;
  }

  /** @return Shape of the weight tensor */
  const std::vector<int64_t>& shape() const { return shape_; }

  /** @return Quantization type ("per_tensor" or "per_channel") */
  const std::string& quantization() const { return quantization_; }

  /** @return Quantized weight values */
  const QTensor& values() const { return values_; }

  /** @return Scale factor for per-tensor quantization */
  float scale() const { return scale_; }

  /** @return Scale factors for per-channel quantization */
  const std::vector<float>& scales() const { return scales_; }

  /** @return Axis along which per-channel quantization is performed */
  int axis() const { return axis_; }

 private:
  std::vector<int64_t> shape_;
  std::string quantization_;
  QTensor values_;
  float scale_{0.0f};
  std::vector<float> scales_;
  int axis_{0};
};

/**
 * @brief Base class for all neural network operators
 *
 * @tparam T Data type of the tensor elements (e.g., float, int8_t)
 */
template <typename InputT, typename OutputT>
class Operator {
 public:
  /** @brief Virtual destructor for proper cleanup of derived classes */
  virtual ~Operator() = default;

  /**
   * @brief Performs the forward computation of the operator
   *
   * @param input Input tensor to the operator
   * @param output Output tensor where results will be stored
   * @throws std::runtime_error If computation fails or dimensions mismatch
   */
  virtual void Forward(const Tensor<InputT>& input,
                       Tensor<OutputT>& output) = 0;

  /** @brief Name identifier of the operator */
  std::string name;

  /** @brief Type identifier of the operator */
  std::string type;

  /** @brief Type definition for input tensor */
  using input_type = InputT;

  /** @brief Type definition for output tensor */
  using output_type = OutputT;
};

/** @brief Smart pointer type for operator instances */
template <typename InputT, typename OutputT>
using OperatorPtr = std::unique_ptr<Operator<InputT, OutputT>>;

// Common type aliases
using QuantOperator = Operator<int8_t, int8_t>;
using QuantOperatorPtr = OperatorPtr<int8_t, int8_t>;

// Add mixed precision operator types if needed
using QuantStubOperator = Operator<float, int8_t>;
using QuantStubOperatorPtr = OperatorPtr<float, int8_t>;

using DeQuantStubOperator = Operator<int8_t, float>;
using DeQuantStubOperatorPtr = OperatorPtr<int8_t, float>;

}  // namespace qnn
