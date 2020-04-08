/**
 * @file hal_test.h
 * @brief Test framework for HAL unit testing
 */

#ifndef HAL_TEST_H
#define HAL_TEST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Test statistics
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Basic assertions
#define HAL_TEST_ASSERT(condition)                 \
  do {                                             \
    total_tests++;                                 \
    if (condition) {                               \
      passed_tests++;                              \
      printf("PASS: %s:%d\n", __FILE__, __LINE__); \
    } else {                                       \
      failed_tests++;                              \
      printf("FAIL: %s:%d\n", __FILE__, __LINE__); \
    }                                              \
  } while (0)

// Equality assertions
#define HAL_TEST_ASSERT_EQUAL(expected, actual) \
  HAL_TEST_ASSERT((expected) == (actual))

#define HAL_TEST_ASSERT_EQUAL_UINT64(expected, actual) \
  HAL_TEST_ASSERT((uint64_t)(expected) == (uint64_t)(actual))

// Pointer assertions
#define HAL_TEST_ASSERT_NOT_NULL(ptr) HAL_TEST_ASSERT((ptr) != NULL)

#define HAL_TEST_ASSERT_NULL(ptr) HAL_TEST_ASSERT((ptr) == NULL)

// Comparison assertions
#define HAL_TEST_ASSERT_NOT_EQUAL(expected, actual) \
  HAL_TEST_ASSERT((expected) != (actual))

#define HAL_TEST_ASSERT_GREATER_OR_EQUAL_UINT64(expected, actual) \
  HAL_TEST_ASSERT((uint64_t)(actual) >= (uint64_t)(expected))

#define HAL_TEST_ASSERT_LESS_THAN_UINT64(expected, actual) \
  HAL_TEST_ASSERT((uint64_t)(actual) < (uint64_t)(expected))

#define HAL_TEST_ASSERT_LESS_THAN(expected, actual) \
  HAL_TEST_ASSERT((actual) < (expected))

// Test control
#define HAL_TEST_BEGIN()             \
  do {                               \
    total_tests = 0;                 \
    passed_tests = 0;                \
    failed_tests = 0;                \
    printf("\nStarting tests...\n"); \
  } while (0)

#define HAL_TEST_END()                    \
  do {                                    \
    printf("\nTest Summary:\n");          \
    printf("Total:  %d\n", total_tests);  \
    printf("Passed: %d\n", passed_tests); \
    printf("Failed: %d\n", failed_tests); \
    return failed_tests;                  \
  } while (0)

#define HAL_TEST_RUN(test_func)              \
  do {                                       \
    printf("\nRunning %s...\n", #test_func); \
    test_func();                             \
  } while (0)

#endif /* HAL_TEST_H */