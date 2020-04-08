# Set paths for pre-built driver
set(DRIVER_ROOT ${PROJECT_SOURCE_DIR}/../driver)
set(DRIVER_INCLUDE_DIR ${DRIVER_ROOT}/include)
set(DRIVER_LIB_DIR ${DRIVER_ROOT}/lib)

# Create imported target for the pre-built driver library
add_library(accel_driver SHARED IMPORTED)
set_target_properties(accel_driver PROPERTIES
    IMPORTED_LOCATION ${DRIVER_LIB_DIR}/libaccel_driver.so
    INTERFACE_INCLUDE_DIRECTORIES ${DRIVER_INCLUDE_DIR}
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
)

# Add compile definitions to ensure proper C/C++ interop
target_compile_definitions(accel_driver INTERFACE 
    EXTERN_C_LINKAGE
)

# Verify that the library exists
if(NOT EXISTS ${DRIVER_LIB_DIR}/libaccel_driver.so)
    message(FATAL_ERROR "Driver library not found at ${DRIVER_LIB_DIR}/libaccel_driver.so. Please build the driver first.")
endif()