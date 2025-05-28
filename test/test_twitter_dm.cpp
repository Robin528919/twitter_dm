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
            "auth_token=08d2becbe434573c5f3542d1cd948924b1469dc7;guest_id_ads=v1%3A174816983876107511;Max-Age=157680000;Path=/;Domain=.x.com;SameSite=None;guest_id_marketing=v1%3A174816983876107511;lang=en;personalization_id=\"v1_gcQA7E7wC4KTVDcjyZby7w==\";guest_id=v1%3A174816983876107511;twid=u%3D1917054916222152704;ct0=cb357d1748ee410952747c102ebdf6f1701b7605313801d6e1b21a8a37592f4bb6794fa79d570000b150d53e0568b3ec1d7bc526ae023ae966a3ac4bed36000f37622eaa93accb14587188473542fde5;__cf_bm=vX.hTdTSl_qZUzTiCrv4r4NFAQ3AQBfNQ5NyjWm.1sY-1748169838-1.0.1.1-MVefT4wHtJivyh_9CdGSBNyEaPdPwhmGEihDsEMBUmunKYOKncRd.z3R5iAir20FndMHTStMOSLIfL5.VzVR7ocA9akNzIQsU_ErUzqV184;path=/;domain=.x.com;";


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
        std::vector<std::string> user_ids = {"123456789", "", "987654321"};

        // 这个测试会实际尝试发送请求，但由于使用模拟cookies，会失败
        // 主要测试空用户ID的处理逻辑
        EXPECT_NO_THROW({
            auto batch_result = twitter_client->sendBatchDirectMessages(user_ids, "test message");
            EXPECT_EQ(batch_result.results.size(), 3);
            EXPECT_EQ(batch_result.failure_count, 3); // 假设所有模拟请求都失败，包括空ID

            // 检查空用户ID的结果
            bool found_empty_user_error = false;
            for (const auto& result : batch_result.results) {
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
