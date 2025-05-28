/**
 * @file example.cpp
 * @brief Twitter私信批量并发发送库的使用示例
 * @author 系统生成
 * @date 2024
 */

#include "twitter_dm.h"
#include <iostream>
#include <vector>
#include <spdlog/spdlog.h>

int main() {
    try {
        // 示例cookies（请替换为真实的cookies）
        std::string cookies = "ct0=your_csrf_token_here; auth_token=your_auth_token_here; other_cookies=values";
        
        // 创建Twitter客户端实例
        twitter_dm::Twitter twitter_client(cookies);
        
        // 设置日志级别为调试模式
        twitter_client.setLogLevel(spdlog::level::debug);
        
        std::cout << "=== Twitter私信批量发送示例 ===\n" << std::endl;
        
        // 示例1: 发送单条私信
        std::cout << "1. 发送单条私信示例:" << std::endl;
        std::string target_user_id = "123456789";  // 目标用户ID
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
        std::vector<std::string> user_ids = {
            "123456789",
            "987654321",
            "555666777",
            "111222333"
        };
        
        std::string batch_message = "Hello! 这是一条批量发送的测试私信。";
        
        auto batch_results = twitter_client.sendBatchDirectMessages(user_ids, batch_message);
        
        // 统计结果
        int success_count = 0;
        int failed_count = 0;
        
        std::cout << "批量发送结果:" << std::endl;
        for (const auto& result : batch_results) {
            if (result.success) {
                std::cout << "✅ 用户 " << result.user_id << ": 发送成功" << std::endl;
                success_count++;
            } else {
                std::cout << "❌ 用户 " << result.user_id << ": 发送失败 - " << result.error_msg << std::endl;
                failed_count++;
            }
        }
        
        std::cout << "\n📊 批量发送统计:" << std::endl;
        std::cout << "总数: " << batch_results.size() << std::endl;
        std::cout << "成功: " << success_count << std::endl;
        std::cout << "失败: " << failed_count << std::endl;
        
        // 示例3: 验证cookies
        std::cout << "\n3. Cookies验证:" << std::endl;
        if (twitter_client.validateCookies()) {
            std::cout << "✅ Cookies格式有效" << std::endl;
        } else {
            std::cout << "❌ Cookies格式无效" << std::endl;
        }
        
    } catch (const std::invalid_argument& e) {
        std::cerr << "❌ 参数错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::cerr << "❌ 运行时错误: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "❌ 未知错误: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== 示例程序执行完成 ===" << std::endl;
    return 0;
}