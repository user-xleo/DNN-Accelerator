/**
 * @file padding.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Padding operator
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once
#include "operator.hpp"

namespace qnn {

template <typename InputT, typename OutputT>
class Padding : public Operator<InputT, OutputT> {
 public:
  void set_pad_height(int height) { pad_height_ = height; }
  void set_pad_width(int width) { pad_width_ = width; }
  void set_pad_value(InputT value) { pad_value_ = value; }

  /**
   * @brief Loads Padding operator from JSON configuration
   *
   * @param j JSON object containing operator parameters
   * @return Unique pointer to created operator
   */
  static OperatorPtr<InputT, OutputT> LoadFromJson(const json& j) {
    auto op = std::make_unique<Padding<InputT, OutputT>>();
    op->name = j["name"].get<std::string>();
    op->type = "Padding";
    op->set_pad_height(j["pad_height"].get<int>());
    op->set_pad_width(j["pad_width"].get<int>());
    op->set_pad_value(j.value("pad_value", 0));
    return op;
  }

  /**
   * @brief Performs padding of input tensor
   *
   * @param input Input tensor of shape [N, C, H, W]
   * @param output Output tensor of shape [N, C, H + pad_height * 2, W +
   * pad_width * 2]
   * @throws std::runtime_error If input dimensions are invalid
   */
  void Forward(const Tensor<InputT>& input, Tensor<OutputT>& output) override {
    const auto& in_shape = input.shape();
    if (in_shape.size() != 4) {
      throw std::runtime_error("Input tensor must be 4D [N,C,H,W]");
    }

    // Calculate output shape
    std::vector<size_t> out_shape = {
        in_shape[0],                    // N
        in_shape[1],                    // C
        in_shape[2] + pad_height_ * 2,  // H + pad_height
        in_shape[3] + pad_width_ * 2    // W + pad_width
    };
    output.resize(out_shape);

#ifdef BUILD_DEBUG
    spdlog::debug("--------------------------------");
    spdlog::debug("Padding Operator Forward");
    spdlog::debug("Input Shape: [{}]", fmt::join(input.shape(), ", "));
    spdlog::debug("Output Shape: [{}]", fmt::join(output.shape(), ", "));
    spdlog::debug("Pad Height: {}", pad_height_);
    spdlog::debug("Pad Width: {}", pad_width_);
    spdlog::debug("Pad Value: {}", pad_value_);
    spdlog::debug("--------------------------------");
#endif

    // Calculate padding on each side
    int pad_top = pad_height_;
    int pad_bottom = pad_height_;
    int pad_left = pad_width_;
    int pad_right = pad_width_;

    // Get dimensions
    size_t batch = in_shape[0];
    size_t channels = in_shape[1];
    size_t in_height = in_shape[2];
    size_t in_width = in_shape[3];
    size_t out_height = out_shape[2];
    size_t out_width = out_shape[3];

    // Fill output with padding value
    std::fill(output.data(), output.data() + output.size(),
              static_cast<OutputT>(pad_value_));

    // Copy input data to the padded output
    for (size_t n = 0; n < batch; n++) {
      for (size_t c = 0; c < channels; c++) {
        for (size_t h = 0; h < in_height; h++) {
          for (size_t w = 0; w < in_width; w++) {
            size_t in_idx = ((n * channels + c) * in_height + h) * in_width + w;
            size_t out_idx =
                ((n * channels + c) * out_height + (h + pad_top)) * out_width +
                (w + pad_left);
            output.data()[out_idx] = static_cast<OutputT>(input.data()[in_idx]);
          }
        }
      }
    }
  }

 private:
  int pad_height_;
  int pad_width_;
  InputT pad_value_;
};

}  // namespace qnn