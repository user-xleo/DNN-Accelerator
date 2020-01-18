# Add executable
add_executable(inference demo.cc)

# Add include directories
target_include_directories(inference 
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
        ${stb_SOURCE_DIR}
)

# Link libraries
target_link_libraries(inference 
    PRIVATE 
        libqnn
        fmt::fmt
)