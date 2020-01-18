FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${json_SOURCE_DIR}/include)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)