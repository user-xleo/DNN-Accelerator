/**
 * @file maxpool2d.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief 2D Max Pooling operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include "operator.hpp"

namespace qnn {

/**
 * @brief 2D Max Pooling operator
 *
 * Performs max pooling over an input tensor, reducing spatial dimensions
 * by selecting maximum values in pooling windows.
 *
 * @tparam T Data type of the tensor elements (e.g., float, int8_t)
 */
template <typename InputT, typename OutputT>
class MaxPool2d : public Operator<InputT, OutputT> {
 public:
  /**
   * @brief Creates MaxPool2d operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   * @throws json::exception If required parameters are missing
   */
  static OperatorPtr<InputT, OutputT> LoadFromJson(const json& j) {
    auto op = std::make_unique<MaxPool2d<InputT, OutputT>>();
    op->name = j["name"].get<std::string>();
    op->type = "MaxPool2d";
    op->kernel_size_ = j["kernel_size"].get<int>();
    op->stride_ = j["stride"].get<int>();
    op->padding_ = j.value("padding", 0);
    return op;
  }

  /**
   * @brief Performs max pooling computation
   *
   * @param input Input tensor of shape [N, C, H, W]
   * @param output Output tensor of shape [N, C, H_out, W_out]
   * @throws std::runtime_error If input dimensions are invalid
   */
  void Forward(const Tensor<InputT>& input, Tensor<OutputT>& output) override {
    const auto& in_shape = input.shape();
    if (in_shape.size() != 4) {
      throw std::runtime_error("Input tensor must be 4D [N,C,H,W]");
    }

    // Get input dimensions
    size_t batch = in_shape[0];
    size_t channels = in_shape[1];
    size_t in_height = in_shape[2];
    size_t in_width = in_shape[3];

    // Calculate output dimensions
    size_t out_height = (in_height - kernel_size_) / stride_ + 1;
    size_t out_width = (in_width - kernel_size_) / stride_ + 1;

    // Resize output tensor
    std::vector<size_t> out_shape = {batch, channels, out_height, out_width};
    output.resize(out_shape);
    output.set_scale(input.scale());

#ifdef BUILD_DEBUG
    spdlog::debug("--------------------------------");
    spdlog::debug("MaxPool-2D Operator Forward");
    spdlog::debug("Input Shape: [{}]", fmt::join(input.shape(), ", "));
    spdlog::debug("Output Shape: [{}]", fmt::join(output.shape(), ", "));
    spdlog::debug("Kernel Size: {}", kernel_size_);
    spdlog::debug("Stride: {}", stride_);
    spdlog::debug("Padding: {}", padding_);
    spdlog::debug("--------------------------------");
#endif

    // Perform max pooling
    for (size_t n = 0; n < batch; n++) {
      for (size_t c = 0; c < channels; c++) {
        for (size_t oh = 0; oh < out_height; oh++) {
          for (size_t ow = 0; ow < out_width; ow++) {
            // Initialize with minimum value for the type
            InputT max_val = std::numeric_limits<InputT>::lowest();

            // Find maximum in the pooling window
            for (size_t kh = 0; kh < kernel_size_; kh++) {
              for (size_t kw = 0; kw < kernel_size_; kw++) {
                size_t ih = oh * stride_ + kh;
                size_t iw = ow * stride_ + kw;

                size_t in_idx =
                    ((n * channels + c) * in_height + ih) * in_width + iw;
                max_val = std::max(max_val, input.data()[in_idx]);
              }
            }

            // Store the maximum value
            size_t out_idx =
                ((n * channels + c) * out_height + oh) * out_width + ow;
            output.data()[out_idx] = static_cast<OutputT>(max_val);
          }
        }
      }
    }
  }

 private:
  /** @brief Size of the pooling window */
  int kernel_size_;

  /** @brief Stride of the pooling operation */
  int stride_;

  /** @brief Padding size */
  int padding_;
};

}  // namespace qnn