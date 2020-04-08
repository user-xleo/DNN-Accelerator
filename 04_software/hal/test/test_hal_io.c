/**
 * @file test_hal_io.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Unit tests for HAL I/O operations
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_base.h"
#include "hal_io.h"
#include "hal_test.h"

static hal_context_t* ctx;

/**
 * @brief Set up test environment before each test
 */
static void set_up(void) {
  ctx = hal_init("/dev/accelerator0");
  HAL_TEST_ASSERT_NOT_NULL(ctx);
}

/**
 * @brief Clean up test environment after each test
 */
static void tear_down(void) { hal_cleanup(ctx); }

/**
 * @brief Test basic I/O operations
 */
static void test_hal_io_basic(void) {
  set_up();

  // Test wait for ready
  HAL_TEST_ASSERT(hal_wait_for_ready(ctx));

  // Test status checks
  HAL_TEST_ASSERT(hal_is_ready(ctx));
  HAL_TEST_ASSERT(!hal_is_busy(ctx));
  HAL_TEST_ASSERT(!hal_is_error(ctx));

  tear_down();
}

/**
 * @brief Test status transitions
 */
static void test_hal_io_status(void) {
  set_up();

  // Test initial status
  HAL_TEST_ASSERT_EQUAL(HAL_STATUS_READY, hal_get_status(ctx));

  // Test setting status
  hal_set_status(ctx, HAL_STATUS_BUSY);
  HAL_TEST_ASSERT_EQUAL(HAL_STATUS_BUSY, hal_get_status(ctx));
  HAL_TEST_ASSERT(hal_is_busy(ctx));

  hal_set_status(ctx, HAL_STATUS_COMPLETE);
  HAL_TEST_ASSERT_EQUAL(HAL_STATUS_COMPLETE, hal_get_status(ctx));
  HAL_TEST_ASSERT(!hal_is_busy(ctx));

  tear_down();
}

/**
 * @brief Test error handling
 */
static void test_hal_io_error(void) {
  set_up();

  // Test error status
  hal_set_status(ctx, HAL_STATUS_ERROR);
  HAL_TEST_ASSERT(hal_is_error(ctx));
  HAL_TEST_ASSERT(!hal_is_ready(ctx));
  HAL_TEST_ASSERT(!hal_is_busy(ctx));

  // Test error recovery
  hal_set_status(ctx, HAL_STATUS_READY);
  HAL_TEST_ASSERT(!hal_is_error(ctx));
  HAL_TEST_ASSERT(hal_is_ready(ctx));

  tear_down();
}

/**
 * @brief Test invalid parameters
 */
static void test_hal_io_invalid_params(void) {
  // Test null context
  HAL_TEST_ASSERT(!hal_wait_for_ready(NULL));
  HAL_TEST_ASSERT(!hal_is_ready(NULL));
  HAL_TEST_ASSERT(!hal_is_busy(NULL));
  HAL_TEST_ASSERT(!hal_is_error(NULL));
  HAL_TEST_ASSERT_EQUAL(0, hal_get_status(NULL));
}

int main(void) {
  HAL_TEST_BEGIN();

  HAL_TEST_RUN(test_hal_io_basic);
  HAL_TEST_RUN(test_hal_io_status);
  HAL_TEST_RUN(test_hal_io_error);
  HAL_TEST_RUN(test_hal_io_invalid_params);

  HAL_TEST_END();
}