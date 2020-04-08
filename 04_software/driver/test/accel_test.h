/**
 * @file accel_test.h
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Test framework for accelerator driver unit testing
 * @version 1.0.0
 * @date 2020-03-30
 */

#ifndef ACCEL_TEST_H
#define ACCEL_TEST_H

#include <stdio.h>

// Test statistics
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Basic assertions
#define ACCEL_TEST_ASSERT(condition)               \
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
#define ACCEL_TEST_ASSERT_EQUAL(expected, actual) \
  ACCEL_TEST_ASSERT((expected) == (actual))

// Pointer assertions
#define ACCEL_TEST_ASSERT_NOT_NULL(ptr) ACCEL_TEST_ASSERT((ptr) != NULL)

#define ACCEL_TEST_ASSERT_NULL(ptr) ACCEL_TEST_ASSERT((ptr) == NULL)

// Test control
#define ACCEL_TEST_BEGIN()           \
  do {                               \
    total_tests = 0;                 \
    passed_tests = 0;                \
    failed_tests = 0;                \
    printf("\nStarting tests...\n"); \
  } while (0)

#define ACCEL_TEST_END()                  \
  do {                                    \
    printf("\nTest Summary:\n");          \
    printf("Total:  %d\n", total_tests);  \
    printf("Passed: %d\n", passed_tests); \
    printf("Failed: %d\n", failed_tests); \
    return failed_tests;                  \
  } while (0)

#define ACCEL_TEST_RUN(test_func)            \
  do {                                       \
    printf("\nRunning %s...\n", #test_func); \
    test_func();                             \
  } while (0)

#endif /* ACCEL_TEST_H */