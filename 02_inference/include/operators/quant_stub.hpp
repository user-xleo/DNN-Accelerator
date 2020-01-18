/**
 * @file quant_stub.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Quantization stub operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include <cmath>

#include "operator.hpp"

namespace qnn {

/**
 * @brief Quantization stub operator
 *
 * Handles quantization of floating-point tensors to integer tensors.
 * Used at the beginning of quantized neural networks.
 */
class QuantStub : public Operator<float, int8_t> {
 public:
  /**
   * @brief Creates QuantStub operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   * @throws json::exception If required parameters are missing
   */
  static OperatorPtr<float, int8_t> LoadFromJson(const json& j) {
    auto op = std::make_unique<QuantStub>();
    op->scale_ = j["scale"].get<float>();
    op->name = j["name"].get<std::string>();
    op->type = "QuantStub";
    return op;
  }

  /**
   * @brief Performs tensor quantization
   *
   * @param input Floating-point input tensor
   * @param output Quantized output tensor
   * @throws std::runtime_error If quantization parameters are invalid
   */
  void Forward(const Tensor<float>& input, Tensor<int8_t>& output) override {
    // Convert shape types properly
    std::vector<size_t> out_shape(input.shape().begin(), input.shape().end());
    output.resize(out_shape);
    output.set_scale(scale_);

#ifdef BUILD_DEBUG
    spdlog::debug("--------------------------------");
    spdlog::debug("QuantStub Operator Forward");
    spdlog::debug("Input Shape: [{}]", fmt::join(input.shape(), ", "));
    spdlog::debug("Output Shape: [{}]", fmt::join(output.shape(), ", "));
    spdlog::debug("Scale: {}", scale_);
    spdlog::debug("--------------------------------");
#endif

    for (size_t i = 0; i < input.size(); ++i) {
      float temp = (input.data()[i] / scale_);
      output.data()[i] =
          std::clamp(static_cast<int8_t>(std::round(temp)),
                     static_cast<int8_t>(-128), static_cast<int8_t>(127));
    }
  }

 private:
  /** @brief Quantization scale factor */
  float scale_;
};

}  // namespace qnn