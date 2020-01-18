/**
 * @file linear.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Linear operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include "operator.hpp"

namespace qnn {

/**
 * @brief Linear (fully connected) operator
 *
 * Implements a linear transformation: y = xW^T + b
 * Supports both float and quantized computation.
 *
 * @tparam InputT Data type of the input tensor elements (e.g., float, int8_t)
 * @tparam OutputT Data type of the output tensor elements (e.g., float, int8_t)
 */
template <typename InputT, typename OutputT>
class Linear : public Operator<InputT, OutputT> {
 public:
  /**
   * @brief Creates Linear operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   * @throws json::exception If required parameters are missing
   */
  static OperatorPtr<InputT, OutputT> LoadFromJson(const json& j) {
    auto op = std::make_unique<Linear<InputT, OutputT>>();
    op->name = j["name"].get<std::string>();
    op->type = "Linear";

    // Parse weights and bias
    if (j.contains("weight")) {
      op->weight_ = WeightInfo::LoadFromJson(j["weight"]);
    }

    // Parse bias
    if (j.contains("bias")) {
      const auto& bias = j["bias"];
      const auto& values = bias["values"].get<std::vector<float>>();
      op->bias_.assign(values.begin(), values.end());
    }

    // Parse quantization parameters
    if (j.contains("scale")) {
      op->scale_ = j["scale"].get<float>();
    }

    return op;
  }

  /**
   * @brief Performs linear transformation
   *
   * @param input Input tensor of shape [batch_size, in_features_]
   * @param output Output tensor of shape [batch_size, out_features_]
   * @throws std::runtime_error If input dimensions are invalid
   */
  void Forward(const Tensor<int8_t>& input, Tensor<int8_t>& output) override {
    // First dimension is always batch size
    const size_t batch_size = input.shape()[0];

    // Calculate flattened features (multiply all dimensions except batch)
    size_t in_features = 1;
    for (size_t i = 1; i < input.shape().size(); ++i) {
      in_features *= input.shape()[i];
    }

    // Validate input dimensions
    if (in_features != weight_.shape()[1]) {
      throw std::runtime_error(
          "Input features dimension doesn't match weight matrix");
    }

    const size_t out_features = weight_.shape()[0];

    // Resize output tensor to [batch_size, out_features]
    output.resize(std::vector<size_t>{batch_size, out_features});
    output.set_scale(scale_);

#ifdef BUILD_DEBUG
    spdlog::debug("--------------------------------");
    spdlog::debug("Linear Operator Forward");
    spdlog::debug("Input Shape: [{}]", fmt::join(input.shape(), ", "));
    spdlog::debug("Output Shape: [{}]", fmt::join(output.shape(), ", "));
    spdlog::debug("Input Scale: {}", input.scale());
    spdlog::debug("Output Scale: {}", output.scale());
    spdlog::debug("In Features: {}", in_features_);
    spdlog::debug("Out Features: {}", out_features_);
    spdlog::debug("--------------------------------");
#endif

    // Perform matrix multiplication: y = xW^T + b
    for (size_t b = 0; b < batch_size; ++b) {
      for (size_t o = 0; o < out_features; ++o) {
        float acc = 0.0f;

        // Compute dot product
        for (size_t i = 0; i < in_features; ++i) {
          acc += static_cast<float>(input.data()[b * in_features + i]) *
                 static_cast<float>(weight_.values()[o * in_features + i]);
        }

        if (!bias_.empty()) {
          // Bias is adjusted by its scale
          acc += bias_[o] / (weight_.scales()[o] * input.scale());
        }

        // Apply output scale
        acc = acc * (weight_.scales()[o] * input.scale()) / output.scale();

        // Clamp the result to the int8 range
        acc = std::min(std::max(acc, -128.0f), 127.0f);

        // Store the result
        output.data()[b * out_features + o] =
            static_cast<int8_t>(std::round(acc));
      }
    }
  }

 private:
  /** @brief Number of input features */
  int in_features_;

  /** @brief Number of output features */
  int out_features_;

  /** @brief Weight matrix */
  WeightInfo weight_;

  /** @brief Optional bias terms */
  std::vector<float> bias_;

  /** @brief Quantization scale */
  float scale_;
};
}  // namespace qnn