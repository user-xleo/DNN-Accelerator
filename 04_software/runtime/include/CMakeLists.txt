add_library(accel_runtime INTERFACE)

target_include_directories(accel_runtime
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DRIVER_INCLUDE_DIR}
)

# Link against driver library
target_link_libraries(accel_runtime
  INTERFACE
    accel_driver
)