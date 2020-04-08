/**
 * @file test_accel.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Unit tests for basic accelerator driver functionality
 * @version 1.0.0
 * @date 2020-03-30
 */

#include "accel.h"
#include "accel_test.h"

static void test_init_cleanup(void) {
  // Test initialization
  accel_status_t status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test double initialization
  status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test invalid device path
  status = accel_init(NULL);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_INVALID_PARAM);

  // Cleanup
  accel_cleanup();
}

static void test_buffer_management(void) {
  // Initialize
  accel_status_t status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test buffer allocation
  accel_buffer_t* buffer = accel_alloc_buffer(4096);
  ACCEL_TEST_ASSERT_NOT_NULL(buffer);
  ACCEL_TEST_ASSERT_NOT_NULL(buffer->host_addr);
  ACCEL_TEST_ASSERT(buffer->dev_addr != 0);
  ACCEL_TEST_ASSERT_EQUAL(4096, buffer->size);

  // Test zero size allocation
  accel_buffer_t* zero_buffer = accel_alloc_buffer(0);
  ACCEL_TEST_ASSERT_NULL(zero_buffer);

  // Free buffer
  accel_free_buffer(buffer);

  // Test NULL free
  accel_free_buffer(NULL);

  accel_cleanup();
}

static void test_operation_submission(void) {
  // Initialize
  accel_status_t status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Allocate buffers
  accel_buffer_t* input = accel_alloc_buffer(1024);
  accel_buffer_t* output = accel_alloc_buffer(1024);
  accel_buffer_t* weights = accel_alloc_buffer(1024);
  ACCEL_TEST_ASSERT_NOT_NULL(input);
  ACCEL_TEST_ASSERT_NOT_NULL(output);
  ACCEL_TEST_ASSERT_NOT_NULL(weights);

  // Configure operation
  accel_op_params_t params = {.op_type = ACCEL_OP_MATMUL,
                              .input = *input,
                              .output = *output,
                              .weights = *weights,
                              .flags = 0};

  // Submit operation
  status = accel_submit_op(&params);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Wait for completion
  status = accel_wait_complete(1000);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test invalid parameters
  status = accel_submit_op(NULL);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_INVALID_PARAM);

  // Cleanup
  accel_free_buffer(input);
  accel_free_buffer(output);
  accel_free_buffer(weights);
  accel_cleanup();
}

static void test_error_handling(void) {
  // Test operations before initialization
  accel_buffer_t* buffer = accel_alloc_buffer(1024);
  ACCEL_TEST_ASSERT_NULL(buffer);

  accel_op_params_t params = {0};
  accel_status_t status = accel_submit_op(&params);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_NOT_INITIALIZED);

  // Initialize
  status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test error message
  const char* error = accel_get_error();
  ACCEL_TEST_ASSERT_NOT_NULL(error);

  accel_cleanup();
}

int main(void) {
  ACCEL_TEST_BEGIN();

  ACCEL_TEST_RUN(test_init_cleanup);
  ACCEL_TEST_RUN(test_buffer_management);
  ACCEL_TEST_RUN(test_operation_submission);
  ACCEL_TEST_RUN(test_error_handling);

  ACCEL_TEST_END();
}