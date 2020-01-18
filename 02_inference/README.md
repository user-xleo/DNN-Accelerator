# DNN Inference (INT8) in C++

This project utilizes C++ to implement INT8 Deep Neural Network (DNN) inference, aimed at deepening understanding of subsequent Verilog implementations of DNN accelerators. 

To preserve clarity and simplicity in the code, advanced optimizations (e.g., SIMD, OpenMP) have not been employed.

## Prerequisites

- C++ compiler supporting C++17
- CMake version 3.11 or higher

## How to Use
### Build
```bash
cmake -B build
cmake --build build
```

### Run
```bash
./inference <path_to_model> <path_to_image>
```

## Third Party Libraries

This project relies on the following third-party libraries:

- [fmt](https://github.com/fmtlib/fmt)
  - A modern formatting library.

- [nlohmann/json](https://github.com/nlohmann/json) 
  - A robust library tailored for JSON manipulation in Modern C++.

- [gabime/spdlog](https://github.com/gabime/spdlog)
  - A high-performance C++ logging library.

- [nothings/stb](https://github.com/nothings/stb)
  - A single-file public domain library for image loading and writing.

All dependencies are automatically fetched and configured through CMake FetchContent.

## Project Structure
- **cmake** - CMake modules directory
  - **fetch** - Scripts for fetching external dependencies
    - `nlohmann.cmake` - Fetch script for JSON library
    - `spdlog.cmake` - Fetch script for logging library
    - `stb.cmake` - Fetch script for image loading library
- **include**
  - **operators** - Directory for operator implementations
    - `conv2d.hpp` - Convolution 2D operator
    - `linear.hpp` - Linear/Fully connected layer
    - `maxpool2d.hpp` - Max pooling 2D operator
    - `padding.hpp` - Padding operations
    - `quant_stub.hpp` - Quantization stub
    - `relu.hpp` - ReLU activation function
  - `model.hpp` - Model class definition
  - `operator.hpp` - Base operator interface
  - `operator_factory.hpp` - Operator factory pattern
  - `tensor.hpp` - Tensor class definition
  - `CMakeLists.txt`
- **tutorials**
  - `demo.cc`
  - `CMakeLists.txt`
- `CMakeLists.txt`