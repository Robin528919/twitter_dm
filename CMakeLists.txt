cmake_minimum_required(VERSION 3.16)
project(twitter_dm_static)

set(CMAKE_CXX_STANDARD 20)
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)
# 查找依赖库
find_package(PkgConfig REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)
# 添加cpr依赖
set(ENV{http_proxy} "http://127.0.0.1:7890")
set(ENV{https_proxy} "http://127.0.0.1:7890")
include(FetchContent)
FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)


# 添加头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 创建静态库
add_library(twitter_dm_static STATIC
        library.cpp
        twitter_dm.cpp
)

# 链接依赖库
target_link_libraries(twitter_dm_static
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        cpr::cpr
)

# 设置编译选项
target_compile_options(twitter_dm_static PRIVATE -Wall -Wextra)

# 添加示例程序
add_executable(twitter_dm_example example.cpp)
target_link_libraries(twitter_dm_example
        twitter_dm_static
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        cpr::cpr
)

# 添加测试程序
if (BUILD_TESTING OR ENABLE_TESTING)
    enable_testing()

    add_executable(twitter_dm_test test_twitter_dm.cpp)
    target_link_libraries(twitter_dm_test
            twitter_dm_static
            spdlog::spdlog
            nlohmann_json::nlohmann_json
            cpr::cpr
            GTest::gtest
            GTest::gtest_main
    )

    # 添加测试到CTest
    add_test(NAME TwitterDMTest COMMAND twitter_dm_test)
endif ()

# 安装配置
install(TARGETS twitter_dm_static
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(FILES twitter_dm.h library.h
        DESTINATION include
)
