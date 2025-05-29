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
        # 注意：源文件 twitter_dm.cpp 应该由主 CMakeLists.txt 中的 ${PROJECT_NAME} 目标提供
        # 因此这里不需要再次添加 ../twitter_dm.cpp
)

# 链接依赖库
target_link_libraries(twitter_dm_test
        PRIVATE
        twitter_dm_lib # 链接到静态库 twitter_dm_lib
        GTest::gtest      # 使用 GTest::gtest 而不是 gtest
        GTest::gtest_main # 使用 GTest::gtest_main 而不是 gtest_main
)

# 添加测试到CTest
include(GoogleTest)
gtest_discover_tests(twitter_dm_test)