/**
 * @file test_hal_mem.c
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Unit tests for HAL memory management
 * @version 1.0.0
 * @date 2020-03-28
 */

#include "hal_base.h"
#include "hal_mem.h"
#include "hal_test.h"

static hal_context_t* ctx;
static const size_t TEST_SIZE = 1024;  // 1KB for testing

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
 * @brief Test basic memory allocation and deallocation
 */
static void test_hal_mem_basic_alloc_free(void) {
  set_up();

  void* ptr = hal_mem_alloc(ctx, TEST_SIZE);
  HAL_TEST_ASSERT_NOT_NULL(ptr);

  // Verify alignment
  HAL_TEST_ASSERT((((uintptr_t)ptr & (HAL_MEM_ALIGN - 1)) == 0));

  hal_mem_free(ctx, ptr);

  // Verify available memory is restored
  HAL_TEST_ASSERT_EQUAL_UINT64(HAL_ACCEL_MEM_SIZE, hal_mem_available(ctx));

  tear_down();
}

/**
 * @brief Test multiple allocations and deallocations
 */
static void test_hal_mem_multiple_allocs(void) {
  set_up();

  void* ptr1 = hal_mem_alloc(ctx, TEST_SIZE);
  void* ptr2 = hal_mem_alloc(ctx, TEST_SIZE);
  void* ptr3 = hal_mem_alloc(ctx, TEST_SIZE);

  HAL_TEST_ASSERT_NOT_NULL(ptr1);
  HAL_TEST_ASSERT_NOT_NULL(ptr2);
  HAL_TEST_ASSERT_NOT_NULL(ptr3);

  // Verify pointers are different
  HAL_TEST_ASSERT(ptr1 != ptr2);
  HAL_TEST_ASSERT(ptr2 != ptr3);
  HAL_TEST_ASSERT(ptr1 != ptr3);

  hal_mem_free(ctx, ptr2);  // Free middle block

  // Allocate a new block that should fit in the freed space
  void* ptr4 = hal_mem_alloc(ctx, TEST_SIZE);
  HAL_TEST_ASSERT_NOT_NULL(ptr4);

  hal_mem_free(ctx, ptr1);
  hal_mem_free(ctx, ptr3);
  hal_mem_free(ctx, ptr4);

  tear_down();
}

/**
 * @brief Test virtual to physical address conversion
 */
static void test_hal_virt_to_phys(void) {
  set_up();

  void* ptr = hal_mem_alloc(ctx, TEST_SIZE);
  HAL_TEST_ASSERT_NOT_NULL(ptr);

  uint64_t phys_addr = hal_virt_to_phys(ctx, ptr);
  HAL_TEST_ASSERT_NOT_EQUAL(0, phys_addr);
  HAL_TEST_ASSERT_GREATER_OR_EQUAL_UINT64(HAL_ACCEL_MEM_BASE, phys_addr);
  HAL_TEST_ASSERT_LESS_THAN_UINT64(HAL_ACCEL_MEM_BASE + HAL_ACCEL_MEM_SIZE,
                                   phys_addr);

  hal_mem_free(ctx, ptr);

  tear_down();
}

/**
 * @brief Test allocation with invalid parameters
 */
static void test_hal_mem_invalid_params(void) {
  set_up();

  // Test null context
  HAL_TEST_ASSERT_NULL(hal_mem_alloc(NULL, TEST_SIZE));

  // Test zero size
  HAL_TEST_ASSERT_NULL(hal_mem_alloc(ctx, 0));

  // Test too large size
  HAL_TEST_ASSERT_NULL(hal_mem_alloc(ctx, HAL_ACCEL_MEM_SIZE + 1));

  // Test invalid virtual address
  HAL_TEST_ASSERT_EQUAL_UINT64(0, hal_virt_to_phys(ctx, (void*)0xDEADBEEF));

  tear_down();
}

/**
 * @brief Test memory fragmentation and coalescing
 */
static void test_hal_mem_fragmentation(void) {
  set_up();

  void* ptrs[5];
  const size_t small_size = 256;  // 256 bytes

  // Allocate 5 small blocks
  for (int i = 0; i < 5; i++) {
    ptrs[i] = hal_mem_alloc(ctx, small_size);
    HAL_TEST_ASSERT_NOT_NULL(ptrs[i]);
  }

  // Free alternate blocks
  hal_mem_free(ctx, ptrs[1]);
  hal_mem_free(ctx, ptrs[3]);

  // Try to allocate a block that should fit in two coalesced free blocks
  void* large_ptr = hal_mem_alloc(ctx, small_size * 2);
  HAL_TEST_ASSERT_NOT_NULL(large_ptr);

  // Clean up
  hal_mem_free(ctx, ptrs[0]);
  hal_mem_free(ctx, ptrs[2]);
  hal_mem_free(ctx, ptrs[4]);
  hal_mem_free(ctx, large_ptr);

  tear_down();
}

/**
 * @brief Test available memory tracking
 */
static void test_hal_mem_available(void) {
  set_up();

  size_t initial_available = hal_mem_available(ctx);
  HAL_TEST_ASSERT_EQUAL_UINT64(HAL_ACCEL_MEM_SIZE, initial_available);

  void* ptr = hal_mem_alloc(ctx, TEST_SIZE);
  HAL_TEST_ASSERT_NOT_NULL(ptr);

  size_t after_alloc = hal_mem_available(ctx);
  HAL_TEST_ASSERT_LESS_THAN(initial_available, after_alloc);

  hal_mem_free(ctx, ptr);

  size_t after_free = hal_mem_available(ctx);
  HAL_TEST_ASSERT_EQUAL_UINT64(initial_available, after_free);

  tear_down();
}

int main(void) {
  HAL_TEST_BEGIN();

  HAL_TEST_RUN(test_hal_mem_basic_alloc_free);
  HAL_TEST_RUN(test_hal_mem_multiple_allocs);
  HAL_TEST_RUN(test_hal_virt_to_phys);
  HAL_TEST_RUN(test_hal_mem_invalid_params);
  HAL_TEST_RUN(test_hal_mem_fragmentation);
  HAL_TEST_RUN(test_hal_mem_available);

  HAL_TEST_END();
}
