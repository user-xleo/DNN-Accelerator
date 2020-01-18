/**
 * @file relu.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief ReLU operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include "operator.hpp"

namespace qnn {

/**
 * @brief ReLU (Rectified Linear Unit) operator
 *
 * Implements element-wise ReLU activation function.
 *
 * @tparam T Data type of the tensor elements (e.g., float, int8_t)
 */
template <typename InputT, typename OutputT>
class ReLU : public Operator<InputT, OutputT> {
 public:
  /**
   * @brief Creates ReLU operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   * @throws json::exception If required parameters are missing
   */
  static OperatorPtr<InputT, OutputT> LoadFromJson(const json& j) {
    auto op = std::make_unique<ReLU<InputT, OutputT>>();
    op->name = j["name"].get<std::string>();
    op->type = "ReLU";
    return op;
  }

  /**
   * @brief Performs ReLU activation
   *
   * @param input Input tensor
   * @param output Output tensor of same shape as input
   * @throws std::runtime_error If input dimensions are invalid
   */
  void Forward(const Tensor<InputT>& input, Tensor<OutputT>& output) override {
    // Resize output tensor to match input shape
    output.resize(input.shape());
    output.set_scale(input.scale());

    // Apply ReLU: max(0,x)
    for (size_t i = 0; i < input.size(); ++i) {
      output.data()[i] =
          static_cast<OutputT>(input.data()[i] > 0 ? input.data()[i] : 0);
    }
  }
};
}  // namespace qnn