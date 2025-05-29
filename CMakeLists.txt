cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0148 NEW)
project(twitter_dm_static)

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

pybind11_add_module(${PROJECT_NAME} bindings.cpp twitter_dm.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr nlohmann_json::nlohmann_json spdlog::spdlog)

install(TARGETS ${PROJECT_NAME} DESTINATION .)

set(BUILD_TESTING 1)
# 添加测试
if(BUILD_TESTING)
    # 添加头文件目录
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    # googletest
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    # 添加构建选项：构建动态库 (SHARED) 或静态库 (STATIC)
    option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.a/.lib)" OFF)

    # 创建库
    if(BUILD_SHARED_LIBS)
        message(STATUS "Building shared library")
        add_library(twitter_dm SHARED
                twitter_dm.cpp
        )
        set_target_properties(twitter_dm PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON) # 适用于Windows MSVC，简化导出
        set(LIB_NAME twitter_dm)
    else()
        message(STATUS "Building static library")
        add_library(twitter_dm_static STATIC
                twitter_dm.cpp
        )
        set(LIB_NAME twitter_dm_static)
    endif()

    # 链接依赖库
    target_link_libraries(${LIB_NAME}
            spdlog::spdlog
            nlohmann_json::nlohmann_json
            cpr::cpr
    )
    enable_testing()
    add_executable(twitter_dm_test test/test_twitter_dm.cpp)
    target_link_libraries(twitter_dm_test ${LIB_NAME} GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(twitter_dm_test)
endif()