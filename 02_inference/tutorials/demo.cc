/**
 * @file demo.cc
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief A demo for the INT8 inference of LeNet-5
 * @version 1.0.0
 * @date 2020-01-18
 */

#define STB_IMAGE_IMPLEMENTATION
#include <spdlog/spdlog.h>
#include <stb_image.h>

#include <iostream>

#include "model.hpp"
#include "tensor.hpp"

/**
 * @brief Loads an image and converts it to a tensor
 *
 * @tparam T Data type of the output tensor (e.g., float, int8_t)
 * @param img_path Path to the image file
 * @param normalize Whether to normalize pixel values to [0,1]
 * @return Tensor containing the image data in NCHW format [1, C, H, W]
 * @throws std::runtime_error If image loading fails
 */
template <typename T>
qnn::Tensor<T> load_image(const std::string& img_path) {
  int height, width, channels;
  stbi_uc* img = stbi_load(img_path.c_str(), &width, &height, &channels, 1);

  if (!img) {
    throw std::runtime_error("Failed to load image: " + img_path);
  }

  // Create tensor with shape [1, C, H, W]
  qnn::Tensor<T> tensor({1, 1, height, width});
  T* data = tensor.data();

  // Convert image data to tensor
  size_t idx = 0;
  for (int h = 0; h < height; ++h) {
    for (int w = 0; w < width; ++w) {
      T pixel = static_cast<T>(img[h * width + w]);
      data[idx++] = pixel;
    }
  }

  stbi_image_free(img);
  return tensor;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    spdlog::error("Usage: {} <path_to_model> <path_to_image>", argv[0]);
    return 1;
  }

  try {
    // Load model
    auto model = qnn::Model::loadModel(argv[1]);

    spdlog::info("Model loaded: {}", argv[1]);

    // Load image and create input tensor
    std::string image_path = argv[2];
    auto input = load_image<float>(image_path);
    spdlog::info("Image loaded: {}", image_path);

    // Normalize input tensor / 255.0
    std::transform(input.data(), input.data() + input.size(), input.data(),
                   [](float x) { return x / 255.0f; });

    // Forward pass
    auto output = model.forward(input);

    // Output the argmax prediction
    std::visit(
        [](const auto& tensor) {
          size_t max_index = std::distance(
              tensor.data(),
              std::max_element(tensor.data(), tensor.data() + tensor.size()));
          spdlog::info("Prediction: {}", max_index);
        },
        output);
  } catch (const std::exception& e) {
    spdlog::error("Error: {}", e.what());
    return 1;
  }

  return 0;
}