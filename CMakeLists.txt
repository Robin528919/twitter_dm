cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0148 NEW)
project(twitter_dm)

set(CMAKE_CXX_STANDARD 17)
# 添加cpr依赖
set(ENV{http_proxy} "http://127.0.0.1:7890")
set(ENV{https_proxy} "http://127.0.0.1:7890")
include(FetchContent)

# 添加 spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)
# 添加 nlohmann_json
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)


add_subdirectory(extern/pybind11)

# 创建一个静态库，包含核心逻辑
add_library(twitter_dm_lib STATIC twitter_dm.cpp twitter_dm.h)
# 为静态库链接依赖
target_link_libraries(twitter_dm_lib PUBLIC cpr::cpr nlohmann_json::nlohmann_json spdlog::spdlog)

# 创建 Python 模块，并链接到静态库
pybind11_add_module(${PROJECT_NAME} MODULE bindings.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE twitter_dm_lib)

install(TARGETS ${PROJECT_NAME} twitter_dm_lib DESTINATION .)

# 添加测试
if(BUILD_TESTING)
    # 添加头文件目录，确保测试子目录可以访问
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    enable_testing()
    add_subdirectory(test)
endif()