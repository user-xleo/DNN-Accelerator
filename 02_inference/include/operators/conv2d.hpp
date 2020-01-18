/**
 * @file conv2d.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief 2D Convolution operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include "operator.hpp"
#include "operators/padding.hpp"

namespace qnn {

/**
 * @brief 2D Convolution operator
 *
 * Implements 2D convolution with optional bias addition.
 * Supports both float and quantized computation.
 *
 * @tparam InputT Data type of the input tensor elements (e.g., float, int8_t)
 * @tparam OutputT Data type of the output tensor elements (e.g., float, int8_t)
 */
template <typename InputT, typename OutputT>
class Conv2d : public Operator<InputT, OutputT> {
 public:
  /**
   * @brief Creates Conv2d operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   * @throws json::exception If required parameters are missing
   */
  static OperatorPtr<InputT, OutputT> LoadFromJson(const json& j) {
    auto op = std::make_unique<Conv2d<InputT, OutputT>>();

    // Parse basic parameters
    op->name = j["name"].get<std::string>();
    op->type = "Conv2d";
    op->in_channels_ = j["in_channels"].get<int>();
    op->out_channels_ = j["out_channels"].get<int>();
    op->kernel_size_ = j["kernel_size"].get<int>();
    op->stride_ = j["stride"].get<int>();
    op->padding_ = j["padding"].get<int>();

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
   * @brief Performs 2D convolution computation
   *
   * @param input Input tensor of shape [N, C, H, W]
   * @param output Output tensor of shape [N, out_channels_, H_out, W_out]
   * @throws std::runtime_error If input dimensions are invalid
   */
  void Forward(const Tensor<InputT>& input, Tensor<OutputT>& output) override {
    const auto& in_shape = input.shape();
    if (in_shape.size() != 4) {
      throw std::runtime_error("Input tensor must be 4D [N,C,H,W]");
    }

    // Create and setup padding operator if padding is needed
    Tensor<InputT> padded_input;
    padded_input.set_scale(input.scale());

    if (padding_ > 0) {
      auto padding_op = std::make_unique<Padding<InputT, InputT>>();
      padding_op->set_pad_height(padding_);
      padding_op->set_pad_width(padding_);
      padding_op->set_pad_value(0);
      padding_op->Forward(input, padded_input);
    } else {
      padded_input = input;
    }

    // Get dimensions after padding
    const auto& padded_shape = padded_input.shape();
    size_t batch = padded_shape[0];
    size_t in_height = padded_shape[2];
    size_t in_width = padded_shape[3];
    size_t out_height = (in_height - kernel_size_) / stride_ + 1;
    size_t out_width = (in_width - kernel_size_) / stride_ + 1;

    // Convert to vector for resize
    std::vector<size_t> out_shape = {batch, static_cast<size_t>(out_channels_),
                                     out_height, out_width};
    output.resize(out_shape);
    output.set_scale(scale_);

#ifdef BUILD_DEBUG
    spdlog::debug("--------------------------------");
    spdlog::debug("Conv-2D Operator Forward");
    spdlog::debug("Input Shape: [{}]", fmt::join(input.shape(), ", "));
    spdlog::debug("Output Shape: [{}]", fmt::join(output.shape(), ", "));
    spdlog::debug("Input Scale: {}", input.scale());
    spdlog::debug("Output Scale: {}", output.scale());
    spdlog::debug("Kernel size: {}", kernel_size_);
    spdlog::debug("In channels: {}", in_channels_);
    spdlog::debug("Out channels: {}", out_channels_);
    spdlog::debug("Stride: {}", stride_);
    spdlog::debug("Padding: {}", padding_);
    spdlog::debug("--------------------------------");
#endif

    // Perform convolution on padded input
    for (size_t n = 0; n < batch; n++) {
      for (size_t oc = 0; oc < static_cast<size_t>(out_channels_); oc++) {
        for (size_t oh = 0; oh < out_height; oh++) {
          for (size_t ow = 0; ow < out_width; ow++) {
            float acc = 0.0f;

            for (size_t ic = 0; ic < static_cast<size_t>(in_channels_); ic++) {
              for (size_t kh = 0; kh < kernel_size_; kh++) {
                for (size_t kw = 0; kw < kernel_size_; kw++) {
                  size_t ih = oh * stride_ + kh;
                  size_t iw = ow * stride_ + kw;

                  size_t in_idx =
                      ((n * in_channels_ + ic) * in_height + ih) * in_width +
                      iw;
                  size_t weight_idx =
                      ((oc * in_channels_ + ic) * kernel_size_ + kh) *
                          kernel_size_ +
                      kw;

                  acc += static_cast<float>(padded_input.data()[in_idx]) *
                         static_cast<float>(weight_.values()[weight_idx]);
                }
              }
            }

            if (!bias_.empty()) {
              acc += bias_[oc] / (weight_.scales()[oc] * padded_input.scale());
            }

            // Apply output scale
            acc = acc * (weight_.scales()[oc] * padded_input.scale()) /
                  output.scale();

            // Clamp the result to the int8 range
            acc = std::min(std::max(acc, -128.0f), 127.0f);

            // Store the result
            size_t out_idx =
                ((n * out_channels_ + oc) * out_height + oh) * out_width + ow;
            output.data()[out_idx] = static_cast<int8_t>(std::round(acc));
          }
        }
      }
    }
  }

 private:
  /** @brief Number of input channels */
  int in_channels_;

  /** @brief Number of output channels */
  int out_channels_;

  /** @brief Size of the convolution kernel */
  int kernel_size_;

  /** @brief Stride of the convolution */
  int stride_;

  /** @brief Padding size */
  int padding_;

  /** @brief Convolution weights */
  WeightInfo weight_;

  /** @brief Optional bias terms */
  std::vector<float> bias_;

  /** @brief Scale for quantization */
  float scale_;
};
}  // namespace qnn