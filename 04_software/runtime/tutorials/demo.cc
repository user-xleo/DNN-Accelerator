/**
 * @file demo.cc
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Demo for runtime
 * @version 1.0.0
 * @date 2020-04-08
 */

#include <accel.hpp>

int main() {
  accel::Runtime runtime("/dev/accelerator");
  runtime.Configure(accel::kEnableDma);

  accel::Buffer input(1024);
  accel::Buffer weights(1024);
  accel::Buffer output(1024);

  runtime.MatrixMultiply(input, weights, output);
  return 0;
}