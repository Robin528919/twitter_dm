# 测试目录的CMakeLists.txt文件
# 用于构建Twitter DM库的单元测试

cmake_minimum_required(VERSION 3.16)
include(FetchContent)

# 添加 GTest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# 创建测试可执行文件
add_executable(twitter_dm_test
        test_twitter_dm.cpp
        ../twitter_dm.cpp  # 包含源文件
)

# 链接依赖库
target_link_libraries(twitter_dm_test
        gtest
        gtest_main
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        cpr::cpr
)

## 设置编译选项
#target_compile_options(twitter_dm_test PRIVATE -Wall -Wextra)
#
## 添加测试到CTest
#include(GoogleTest)
#gtest_discover_tests(twitter_dm_test)

# 或者手动添加测试
add_test(NAME TwitterDMUnitTest COMMAND twitter_dm_test)