/**
 * @file example.cpp
 * @brief Twitter私信批量并发发送库的使用示例
 * @author 系统生成
 * @date 2024
 */

#include "twitter_dm.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <spdlog/spdlog.h>

int main() {
    try {
        // 示例cookies（请替换为真实的cookies）
        std::string cookies =
                "auth_token=0370676451cd91a6f8fa964417f7cfec7c253e88;guest_id_ads=v1%3A174816983873484138;Max-Age=157680000;Path=/;Domain=.x.com;SameSite=None;guest_id_marketing=v1%3A174816983873484138;lang=en;personalization_id=\"v1_NJ3xITqHNZDmureywMMGsg==\";guest_id=v1%3A174816983873484138;twid=u%3D1917075365291286528;ct0=76882256d47d6143bddf0687aff6bf8a6fbb9723bf3b204f1dda9c625256718c6fe10c3eb5f6c5f81a1090c591745a423a89e2224726b384247cdfe3fdc60b999991309de4045e4a9533fddc02db50c3;__cf_bm=sjtaC5FCMrr8_6n.ESYirQ3jK_iPLXRSMLdRXE9ygJ4-1748169838-1.0.1.1-Tw2.5do0GyE.rICLs89F_IY3rSJPw.Wz7870uz21kIt8Hx0PwmAgdE8G9lmoV1jcZ5iYWW6sRIG9ydRN67yO19aqWCL8FwRWf7H21A6.ZQY;path=/;domain=.x.com;";

        // 创建Twitter客户端实例
        twitter_dm::Twitter twitter_client(cookies, "http://127.0.0.1:8080");

        // 设置日志级别为调试模式
        twitter_client.setLogLevel(spdlog::level::debug);

        std::cout << "=== Twitter私信批量发送示例 ===\n" << std::endl;

        // 示例1: 发送单条私信
        std::cout << "1. 发送单条私信示例:" << std::endl;
        std::string target_user_id = "1187914373911797760"; // 目标用户ID
        std::string message = "Hello! 这是一条测试私信。";

        auto single_result = twitter_client.sendDirectMessage(target_user_id, message);

        if (single_result.success) {
            std::cout << "✅ 单条私信发送成功!" << std::endl;
        } else {
            std::cout << "❌ 单条私信发送失败: " << single_result.error_msg << std::endl;
        }
        std::cout << std::endl;

        // 示例2: 批量发送私信
        std::cout << "2. 批量发送私信示例:" << std::endl;

        // 从文件中读取用户ID列表
        std::vector<std::string> user_ids;
        std::string user_ids_file = "user_ids.txt"; // 用户ID文件路径
        std::ifstream file(user_ids_file);

        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // 去除行首行尾的空白字符
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);

                // 如果行不为空，则添加到用户ID列表中
                if (!line.empty()) {
                    user_ids.push_back(line);
                }
            }
            file.close();
            std::cout << "从文件 " << user_ids_file << " 中读取到 " << user_ids.size() << " 个用户ID" << std::endl;
        } else {
            std::cout << "⚠️  无法打开文件 " << user_ids_file << "，使用默认用户ID列表" << std::endl;
            // 如果文件不存在，使用默认的用户ID列表
            user_ids = {
                "1187914373911797760",
                "899666707551105025",
                "840669830"
            };
        }

        std::string batch_message = "Hello! 这是一条批量发送的测试私信。";

        // 记录批量发送开始时间
        auto start_time = std::chrono::high_resolution_clock::now();
        std::cout << "开始批量发送私信..." << std::endl;

        auto batch_results = twitter_client.sendBatchDirectMessages(user_ids, batch_message);

        // 记录批量发送结束时间并计算耗时
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

        std::cout << "批量发送结果:" << std::endl;

        std::cout << "\n📊 批量发送统计:" << std::endl;
        std::cout << "总数: " << batch_results.results.size() << std::endl;
        std::cout << "成功: " << batch_results.success_count << std::endl;
        std::cout << "失败: " << batch_results.failure_count << std::endl;
        std::cout << "⏱️  总耗时: " << duration.count() << " 毫秒 (" << duration_seconds.count() << " 秒)" << std::endl;

        // 示例3: 验证cookies
        std::cout << "\n3. Cookies验证:" << std::endl;
        if (twitter_client.validateCookies()) {
            std::cout << "✅ Cookies格式有效" << std::endl;
        } else {
            std::cout << "❌ Cookies格式无效" << std::endl;
        }
    } catch (const std::invalid_argument &e) {
        std::cerr << "❌ 参数错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error &e) {
        std::cerr << "❌ 运行时错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "❌ 未知错误: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n=== 示例程序执行完成 ===" << std::endl;
    return 0;
}
