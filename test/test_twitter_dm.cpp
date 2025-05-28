/**
 * @file test_twitter_dm.cpp
 * @brief Twitter私信功能的单元测试
 * @author 系统生成
 * @date 2024
 */

#include "gtest/gtest.h"
#include "twitter_dm.h"
#include <spdlog/spdlog.h>

namespace {

// 测试用的模拟cookies
const std::string VALID_COOKIES = "ct0=test_csrf_token_12345; auth_token=test_auth_token_67890; other=value";
const std::string INVALID_COOKIES = "invalid_cookie=value";
const std::string EMPTY_COOKIES = "";

/**
 * @brief Twitter类构造函数测试套件
 */
class TwitterConstructorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置日志级别为错误，减少测试输出
        spdlog::set_level(spdlog::level::err);
    }
};

/**
 * @brief Twitter类功能测试套件
 */
class TwitterFunctionalityTest : public ::testing::Test {
protected:
    void SetUp() override {
        spdlog::set_level(spdlog::level::err);
        // 注意：这里使用模拟cookies，实际测试时需要真实cookies
        twitter_client = std::make_unique<twitter_dm::Twitter>(VALID_COOKIES);
    }
    
    std::unique_ptr<twitter_dm::Twitter> twitter_client;
};

// 构造函数测试
TEST_F(TwitterConstructorTest, ValidCookies) {
    EXPECT_NO_THROW({
        twitter_dm::Twitter client(VALID_COOKIES);
    });
}

TEST_F(TwitterConstructorTest, InvalidCookies) {
    EXPECT_THROW({
        twitter_dm::Twitter client(INVALID_COOKIES);
    }, std::invalid_argument);
}

TEST_F(TwitterConstructorTest, EmptyCookies) {
    EXPECT_THROW({
        twitter_dm::Twitter client(EMPTY_COOKIES);
    }, std::invalid_argument);
}

// Cookies验证测试
TEST_F(TwitterFunctionalityTest, ValidateCookies) {
    EXPECT_TRUE(twitter_client->validateCookies());
}

TEST_F(TwitterFunctionalityTest, GetCookies) {
    EXPECT_EQ(twitter_client->getCookies(), VALID_COOKIES);
}

// 参数验证测试
TEST_F(TwitterFunctionalityTest, SendDirectMessageEmptyUserId) {
    EXPECT_THROW({
        twitter_client->sendDirectMessage("", "test message");
    }, std::invalid_argument);
}

TEST_F(TwitterFunctionalityTest, SendDirectMessageEmptyMessage) {
    EXPECT_THROW({
        twitter_client->sendDirectMessage("123456789", "");
    }, std::invalid_argument);
}

TEST_F(TwitterFunctionalityTest, SendDirectMessageTooLongMessage) {
    std::string long_message(10001, 'a'); // 超过10000字符限制
    EXPECT_THROW({
        twitter_client->sendDirectMessage("123456789", long_message);
    }, std::invalid_argument);
}

TEST_F(TwitterFunctionalityTest, SendBatchDirectMessagesEmptyUserIds) {
    std::vector<std::string> empty_ids;
    EXPECT_THROW({
        twitter_client->sendBatchDirectMessages(empty_ids, "test message");
    }, std::invalid_argument);
}

TEST_F(TwitterFunctionalityTest, SendBatchDirectMessagesEmptyMessage) {
    std::vector<std::string> user_ids = {"123456789", "987654321"};
    EXPECT_THROW({
        twitter_client->sendBatchDirectMessages(user_ids, "");
    }, std::invalid_argument);
}

// 日志级别设置测试
TEST_F(TwitterFunctionalityTest, SetLogLevel) {
    EXPECT_NO_THROW({
        twitter_client->setLogLevel(spdlog::level::debug);
        twitter_client->setLogLevel(spdlog::level::info);
        twitter_client->setLogLevel(spdlog::level::warn);
        twitter_client->setLogLevel(spdlog::level::err);
    });
}

// DMResult结构体测试
TEST(DMResultTest, DefaultConstructor) {
    twitter_dm::DMResult result;
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.user_id.empty());
    EXPECT_TRUE(result.message.empty());
    EXPECT_TRUE(result.error_msg.empty());
    EXPECT_EQ(result.http_status, 0);
}

TEST(DMResultTest, ParameterizedConstructor) {
    twitter_dm::DMResult result(true, "123456789", "test message", "no error", 200);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.user_id, "123456789");
    EXPECT_EQ(result.message, "test message");
    EXPECT_EQ(result.error_msg, "no error");
    EXPECT_EQ(result.http_status, 200);
}

// 批量处理空用户ID测试
TEST_F(TwitterFunctionalityTest, SendBatchDirectMessagesWithEmptyUserIds) {
    std::vector<std::string> user_ids = {"123456789", "", "987654321"};
    
    // 这个测试会实际尝试发送请求，但由于使用模拟cookies，会失败
    // 主要测试空用户ID的处理逻辑
    EXPECT_NO_THROW({
        auto results = twitter_client->sendBatchDirectMessages(user_ids, "test message");
        EXPECT_EQ(results.size(), 3);
        
        // 检查空用户ID的结果
        bool found_empty_user_error = false;
        for (const auto& result : results) {
            if (result.user_id.empty() && !result.success) {
                found_empty_user_error = true;
                EXPECT_EQ(result.error_msg, "用户ID为空");
                break;
            }
        }
        EXPECT_TRUE(found_empty_user_error);
    });
}

} // namespace

/**
 * @brief 主测试函数
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return int 测试结果
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "=== Twitter DM Library 单元测试 ===" << std::endl;
    std::cout << "注意：某些测试需要真实的Twitter cookies才能完全通过" << std::endl;
    std::cout << "当前测试主要验证参数验证和基本功能逻辑" << std::endl;
    std::cout << std::endl;
    
    return RUN_ALL_TESTS();
}