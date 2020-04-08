/**
 * @file test_config.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Unit tests for accelerator configuration
 * @version 1.0.0
 * @date 2020-03-30
 */
#include "accel.h"
#include "accel_config.h"
#include "accel_test.h"

static void test_basic_config(void) {
  // Initialize
  accel_status_t status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test configuration
  accel_config_t config = {
      .flags = ACCEL_CONFIG_ENABLE_DMA | ACCEL_CONFIG_SYNC_MODE,
      .num_channels = 2,
      .max_transfer = 0x1000000,
      .timeout_ms = 5000};

  status = accel_configure(&config);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Read back configuration
  accel_config_t read_config = {0};
  status = accel_get_config(&read_config);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Verify configuration
  ACCEL_TEST_ASSERT_EQUAL(config.flags, read_config.flags);
  ACCEL_TEST_ASSERT_EQUAL(config.num_channels, read_config.num_channels);
  ACCEL_TEST_ASSERT_EQUAL(config.max_transfer, read_config.max_transfer);
  ACCEL_TEST_ASSERT_EQUAL(config.timeout_ms, read_config.timeout_ms);

  accel_cleanup();
}

static void test_config_reset(void) {
  // Initialize
  accel_status_t status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Set custom configuration
  accel_config_t config = {.flags = ACCEL_CONFIG_HIGH_PRIORITY,
                           .num_channels = 4,
                           .max_transfer = 0x2000000,
                           .timeout_ms = 10000};

  status = accel_configure(&config);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Reset configuration
  status = accel_reset_config();
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Read back configuration
  accel_config_t read_config = {0};
  status = accel_get_config(&read_config);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Verify default values
  ACCEL_TEST_ASSERT_EQUAL(ACCEL_CONFIG_ENABLE_DMA, read_config.flags);
  ACCEL_TEST_ASSERT_EQUAL(1, read_config.num_channels);
  ACCEL_TEST_ASSERT_EQUAL(0x1000000, read_config.max_transfer);
  ACCEL_TEST_ASSERT_EQUAL(1000, read_config.timeout_ms);

  accel_cleanup();
}

static void test_invalid_config(void) {
  // Test before initialization
  accel_config_t config = {0};
  accel_status_t status = accel_configure(&config);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_NOT_INITIALIZED);

  // Initialize
  status = accel_init("/dev/accelerator0");
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_OK);

  // Test NULL config
  status = accel_configure(NULL);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_INVALID_PARAM);

  status = accel_get_config(NULL);
  ACCEL_TEST_ASSERT(status == ACCEL_STATUS_INVALID_PARAM);

  accel_cleanup();
}

int main(void) {
  ACCEL_TEST_BEGIN();

  ACCEL_TEST_RUN(test_basic_config);
  ACCEL_TEST_RUN(test_config_reset);
  ACCEL_TEST_RUN(test_invalid_config);

  ACCEL_TEST_END();
}