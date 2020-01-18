add_library(libqnn INTERFACE)

target_include_directories(libqnn INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(libqnn INTERFACE 
    nlohmann_json::nlohmann_json 
    spdlog::spdlog
    fmt::fmt
) 