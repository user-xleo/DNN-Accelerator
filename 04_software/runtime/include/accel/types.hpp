/**
 * @file types.hpp
 * @author Leo (zhsleo@outlook.com)
 *
 * @brief Common type definitions for runtime
 * @version 1.0.0
 * @date 2020-04-08
 */

#pragma once

#include <cstdint>

namespace accel {

/**
 * @brief Runtime configuration flags
 */
enum ConfigFlags : uint32_t {
  kEnableDma = ACCEL_CONFIG_ENABLE_DMA,
  kSyncMode = ACCEL_CONFIG_SYNC_MODE,
  kHighPriority = ACCEL_CONFIG_HIGH_PRIORITY
};

}  // namespace accel