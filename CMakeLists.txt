cmake_minimum_required(VERSION 3.10)
project(esp_gallery)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g -O0)

# Main core sources
file(GLOB MAIN_CORE_SRC "src/main/core/*.cpp")
add_library(core_main STATIC ${MAIN_CORE_SRC})

# Test core sources
file(GLOB TEST_CORE_SRC "src/test/core/*.cpp")
add_library(core_test STATIC ${TEST_CORE_SRC})

add_definitions(-DLOCAL_TESTING -DSTB_IMAGE_WRITE_IMPLEMENTATION)

# # Main executable (add your main.cpp or .ino wrapper here)
# add_executable(main_app src/main/main.cpp)
# target_link_libraries(main_app core_main)

# Test executable
add_executable(downscale_utils_test src/test/core/downscale_utils_test.cpp)
target_link_libraries(downscale_utils_test core_main core_test)
target_include_directories(downscale_utils_test PRIVATE ${CMAKE_SOURCE_DIR}/libs/stb)


# copy test resources
file(COPY src/test/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})