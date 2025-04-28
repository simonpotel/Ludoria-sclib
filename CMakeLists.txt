cmake_minimum_required(VERSION 3.10)
project(Ludoria-scbot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")
endif()

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(json)

set(SOURCES
    src/main.cpp
    src/CtS.cpp
    src/StC.cpp
    src/Network.cpp
    src/Config.cpp
    src/utils/Logger.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/configs)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}/configs
    COMMENT "copying configs directory to build directory"
) 