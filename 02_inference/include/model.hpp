/**
 * @file model.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Model class
 * @version 1.0.0
 * @date 2020-01-18
 */

#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <variant>

#include "operator.hpp"
#include "operator_factory.hpp"

namespace qnn {

/**
 * @brief Neural network model container
 *
 * This class represents a neural network model that can be loaded from a JSON
 * file and executed. It maintains a sequence of operators that form the model's
 * computation graph.
 */
class Model {
 public:
  /**
   * @brief Constructor for the Model class
   *
   * This constructor sets the logging level to debug if the build type is
   * debug.
   */
  Model() {
#ifdef BUILD_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    // Remove resize from constructor
  }

  /**
   * @brief Parse a layer from JSON and create appropriate operator
   *
   * @param layer_json JSON object containing layer configuration
   * @return Unique pointer to created operator
   * @throws json::exception If layer configuration is invalid
   * @throws std::runtime_error If layer type is unsupported
   */
  static std::variant<OperatorPtr<float, int8_t>, OperatorPtr<int8_t, int8_t>,
                      OperatorPtr<int8_t, float>>
  parseLayer(const json& layer_json) {
    const auto& type = layer_json["type"].get<std::string>();
    const auto& dtype = layer_json.contains("dtype")
                            ? layer_json["dtype"].get<std::string>()
                            : "torch.qint8";

    if (type == "QuantStub") {
      return QuantStub::LoadFromJson(layer_json);
    }

    if (type == "DeQuantStub") {
      return DeQuantStub::LoadFromJson(layer_json);
    }

    auto createOp = [&]<typename T>() {
      if (type == "Conv2d") return Conv2d<T, T>::LoadFromJson(layer_json);
      if (type == "Linear") return Linear<T, T>::LoadFromJson(layer_json);
      if (type == "MaxPool2d") return MaxPool2d<T, T>::LoadFromJson(layer_json);
      if (type == "ReLU") return ReLU<T, T>::LoadFromJson(layer_json);
      throw std::runtime_error("Unknown operator type: " + type);
    };

    if (dtype == "torch.qint8") {
      return createOp.template operator()<int8_t>();
    }
    throw std::runtime_error("Unknown supported dtype: " + dtype);
  }

  /**
   * @brief Creates a Model instance from a JSON file
   *
   * @param filename Path to the JSON file containing model configuration
   * @return Model instance initialized with operators from the JSON
   * @throws std::runtime_error If file cannot be opened or parsed
   * @throws json::exception If JSON structure is invalid
   *
   * The JSON file should contain:
   * - List of operators with their configurations
   * - Operator connections/graph structure
   * - Model metadata (optional)
   */
  static Model loadModel(const std::string& filename) {
    Model model;

    // Read and parse JSON file
    std::ifstream file(filename);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open model file: " + filename);
    }

    json j;
    file >> j;

    // Parse layers from JSON
    const auto& layers = j["layers"];

    // Reserve space for operators
    model.operators_.reserve(layers.size());

    // Resize intermediate tensors based on number of layers
    model.intermediate_tensors_.resize(layers.size());

    for (const auto& layer : layers) {
      try {
        model.operators_.push_back(parseLayer(layer));
      } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse layer: " +
                                 std::string(e.what()));
      }
    }

    return model;
  }

  /**
   * @brief Performs forward pass through the model
   *
   * @param input Input tensor to the model
   * @param output Output tensor where results will be stored
   * @throws std::runtime_error If model has no operators or computation fails
   *
   * This method:
   * 1. Validates input dimensions
   * 2. Executes each operator in sequence
   * 3. Stores final result in output tensor
   *
   * The input and output tensors must have compatible dimensions with
   * the model's expected input/output shapes.
   */
  std::variant<Tensor<float>, Tensor<int8_t>> forward(
      const Tensor<float>& input) {
    if (operators_.empty()) {
      throw std::runtime_error("No operators in model");
    }

    // Initialize input tensor
    input_tensor_ = std::move(input);

    // Process each operator
    for (size_t i = 0; i < operators_.size(); ++i) {
      const auto& op_variant = operators_[i];
      std::visit(
          [&](const auto& op) {
            using Op = std::remove_reference_t<decltype(*op)>;
            using OpInputT = typename Op::input_type;
            using OpOutputT = typename Op::output_type;

            spdlog::debug("Layer: {} ({})", op->name, op->type);

            if constexpr (std::is_same_v<OpInputT, float> &&
                          std::is_same_v<OpOutputT, int8_t>) {
              op->Forward(input_tensor_, intermediate_tensors_[i]);
            } else if constexpr (std::is_same_v<OpInputT, int8_t> &&
                                 std::is_same_v<OpOutputT, int8_t>) {
              op->Forward(intermediate_tensors_[i - 1],
                          intermediate_tensors_[i]);
            } else if constexpr (std::is_same_v<OpInputT, int8_t> &&
                                 std::is_same_v<OpOutputT, float>) {
              op->Forward(intermediate_tensors_[i - 1], output_tensor_);
            } else {
              throw std::runtime_error("Unsupported operator type: " +
                                       op->type);
            }
          },
          op_variant);
    }

    // Return the final output
    return std::variant<Tensor<float>, Tensor<int8_t>>(
        std::move(output_tensor_));
  }

 private:
  /** @brief Vector of operators that form the model's computation graph */
  std::vector<
      std::variant<OperatorPtr<float, int8_t>, OperatorPtr<int8_t, int8_t>,
                   OperatorPtr<int8_t, float>>>
      operators_;

  // Tensors for input, output and intermediate results
  Tensor<float> input_tensor_;
  Tensor<float> output_tensor_;
  std::vector<Tensor<int8_t>> intermediate_tensors_;
};

}  // namespace qnn