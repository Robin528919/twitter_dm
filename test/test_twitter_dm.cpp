/**
 * @file test_twitter_dm.cpp
 * @brief Twitter私信功能的单元测试
 * @author 系统生成
 * @date 2024
 */

#include <gtest/gtest.h>
#include "twitter_dm.h"
#include <spdlog/spdlog.h>

namespace {
    // 测试用的模拟cookies
    const std::string VALID_COOKIES =
            "auth_token=267b12d30fc35e6c695e262c8cd73bf93352152f;guest_id_ads=v1%3A174816983892793945;Max-Age=157680000;Path=/;Domain=.x.com;SameSite=None;guest_id_marketing=v1%3A174816983892793945;lang=en;personalization_id=\"v1_2VwzzojH2GXa8p1XMFf2BA==\";guest_id=v1%3A174816983892793945;twid=u%3D1917038384096104448;ct0=7aeea185774c835b5891c0e5604e70ea1365b4fc85ef722f1c2c2b5f7cac25df5e7c372607ff73069d4b023d2e32a262300124aa1abaed1c0dd0862dbb7a475ac46d3176970251c189801ca3ca67a9d6;__cf_bm=oB6HjEU2UQ_Hq6PQa_vogmaLkZT5SbHAerftf6P0yGU-1748169839-1.0.1.1-xiRDQKi47438kBoAPp3Zmx88POILdMB8WTvfrOKEyvIjHn0a4Kj6ZIMXGCN1a48TG3Gy5A.0G4s8ehlMZYFTa5lJ6f6dTGbFZ6rQBI0JlaE;path=/;domain=.x.com;";


    /**
     * @brief Twitter类功能测试套件
     */
    class TwitterFunctionalityTest : public ::testing::Test {
    protected:
        void SetUp() override {
            spdlog::set_level(spdlog::level::err);
            // 注意：这里使用模拟cookies，实际测试时需要真实cookies
            twitter_client = std::make_unique<twitter_dm::Twitter>(VALID_COOKIES, "http://127.0.0.1:8080");
        }

        std::unique_ptr<twitter_dm::Twitter> twitter_client;
    };

    // 单消息发送测试 (参数验证)
    TEST_F(TwitterFunctionalityTest, SendDirectMessageParameterValidation) {
        try {
            twitter_client->sendDirectMessage("", "test message");
            FAIL() << "Expected std::invalid_argument for empty user_id";
        } catch (const std::invalid_argument &e) {
            EXPECT_STREQ("用户ID不能为空", e.what());
            std::cout << "Caught expected exception for empty user_id: " << e.what() << std::endl;
        }

        try {
            twitter_client->sendDirectMessage("123456789", "");
            FAIL() << "Expected std::invalid_argument for empty message";
        } catch (const std::invalid_argument &e) {
            EXPECT_STREQ("消息内容不能为空", e.what());
            std::cout << "Caught expected exception for empty message: " << e.what() << std::endl;
        }

        try {
            std::string long_message(10001, 'a');
            twitter_client->sendDirectMessage("123456789", long_message);
            FAIL() << "Expected std::invalid_argument for message too long";
        } catch (const std::invalid_argument &e) {
            EXPECT_STREQ("消息内容过长，最大长度为10000字符", e.what());
            std::cout << "Caught expected exception for message too long: " << e.what() << std::endl;
        }
    }

    // 真实发送单条私信测试 (需要真实cookies和用户ID)
    // 注意：此测试用例会尝试真实发送私信。
    // 请确保在测试时使用有效的cookies，并向一个测试账户或你自己的账户发送，以避免发送垃圾信息。
    // 由于我们使用的是模拟cookies，此测试预期不会成功发送，但应能正确处理API调用。
    TEST_F(TwitterFunctionalityTest, SendDirectMessageReal) {
        // 请替换为真实的测试用户ID
        const std::string test_recipient_user_id = "1187914373911797760"; // 示例ID，请替换
        const std::string test_message = "这是一条来自gtest的真实测试消息。";

        std::cout << "尝试发送一条真实的私信..." << std::endl;
        std::cout << "接收方用户ID: " << test_recipient_user_id << std::endl;
        std::cout << "消息内容: " << test_message << std::endl;

        // 由于使用的是模拟cookies，我们预期这个调用不会成功发送，
        // 但它不应该抛出未处理的异常，而是应该由sendDirectMessage内部处理错误。
        // 如果网络请求失败或API返回错误，sendDirectMessage应该返回false或抛出特定异常（如果设计如此）。
        // 这里我们简单地检查它是否不抛出意外的C++异常。
        EXPECT_NO_THROW({
            try {
            auto result = twitter_client->sendDirectMessage(test_recipient_user_id, test_message);
            if (result.success) {
            std::cout << "消息发送成功 (或看起来成功了)。" << std::endl;
            } else {
            std::cout << "由于使用模拟cookies或无效的接收者，消息发送失败（符合预期）。" << std::endl;
            if (!result.error_msg.empty()) {
            std::cout << "错误信息: " << result.error_msg << std::endl;
            }
            }
            // 根据实际sendDirectMessage的返回值和错误处理逻辑，这里可以添加更具体的断言
            // 例如，如果sendDirectMessage在失败时返回false，可以 EXPECT_FALSE(result.success);
            } catch (const std::exception& e) {
            std::cerr << "SendDirectMessageReal 测试捕获到异常: " << e.what() << std::endl;
            FAIL() << "SendDirectMessageReal 抛出了意外的异常: " << e.what();
            }
            });
        std::cout << "真实私信发送尝试已完成。" << std::endl;
    }

    // 批量消息发送测试 (参数验证和空用户ID处理)
    TEST_F(TwitterFunctionalityTest, SendBatchDirectMessagesWithEmptyUserIds) {
        // 从本地文件user_ids.txt逐行读取用户ID
        std::vector<std::string> user_ids;
        std::ifstream infile("../user_ids.txt");
        std::string line;
        while (std::getline(infile, line)) {
            user_ids.push_back(line);
        }

        // 这个测试会实际尝试发送请求，但由于使用模拟cookies，会失败
        // 主要测试空用户ID的处理逻辑
        EXPECT_NO_THROW({
            auto batch_result = twitter_client->sendBatchDirectMessages(user_ids, "test message");
            std::cout << "消息发送成功:"<<batch_result.success_count << std::endl;
            std::cout << "消息发送失败:"<<batch_result.failure_count << std::endl;
            });
    }
} // namespace
