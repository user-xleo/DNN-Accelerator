/**
 * @file test_hal_init.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Unit tests for HAL initialization
 *
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_base.h"
#include "hal_test.h"

/**
 * @brief Test basic initialization and cleanup
 */
static void test_hal_init_basic(void) {
  hal_context_t* ctx = hal_init("/dev/accelerator0");
  HAL_TEST_ASSERT_NOT_NULL(ctx);
  HAL_TEST_ASSERT_NOT_NULL(ctx->mapped_memory);
  HAL_TEST_ASSERT_NOT_NULL(ctx->accel_memory_base);
  HAL_TEST_ASSERT_EQUAL(HAL_ACCEL_MEM_SIZE, ctx->accel_memory_size);
  hal_cleanup(ctx);
}

/**
 * @brief Test initialization with invalid parameters
 */
static void test_hal_init_invalid_params(void) {
  // Test NULL device path
  hal_context_t* ctx = hal_init(NULL);
  HAL_TEST_ASSERT_NULL(ctx);

  // Test invalid device path
  ctx = hal_init("/dev/nonexistent");
  HAL_TEST_ASSERT_NULL(ctx);
}

/**
 * @brief Test cleanup with invalid parameters
 */
static void test_hal_init_cleanup_null(void) {
  // Should not crash with NULL context
  hal_cleanup(NULL);
}

/**
 * @brief Test multiple init/cleanup cycles
 */
static void test_hal_init_multiple(void) {
  hal_context_t* ctx1 = hal_init("/dev/accelerator0");
  HAL_TEST_ASSERT_NOT_NULL(ctx1);

  hal_context_t* ctx2 = hal_init("/dev/accelerator0");
  HAL_TEST_ASSERT_NOT_NULL(ctx2);

  // Contexts should be different
  HAL_TEST_ASSERT(ctx1 != ctx2);

  hal_cleanup(ctx1);
  hal_cleanup(ctx2);
}

int main(void) {
  HAL_TEST_BEGIN();

  HAL_TEST_RUN(test_hal_init_basic);
  HAL_TEST_RUN(test_hal_init_invalid_params);
  HAL_TEST_RUN(test_hal_init_cleanup_null);
  HAL_TEST_RUN(test_hal_init_multiple);

  HAL_TEST_END();
}