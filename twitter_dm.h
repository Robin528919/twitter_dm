/**
 * @file twitter_dm.h
 * @brief Twitter私信批量并发发送功能的核心类定义
 * @author 系统生成
 * @date 2024
 */

#ifndef TWITTER_DM_H
#define TWITTER_DM_H

#include <string>
#include <vector>
#include <memory>
#include <future>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace twitter_dm {
    /**
     * @brief Twitter私信发送结果结构体
     */


    struct DMResult {
        bool success; // 发送是否成功
        std::string user_id; // 目标用户ID
        std::string message; // 发送的消息内容
        std::string error_msg; // 错误信息（如果有）
        int http_status; // HTTP状态码

        /**
         * @brief 构造函数
         * @param success 是否成功
         * @param user_id 用户ID
         * @param message 消息内容
         * @param error_msg 错误信息
         * @param http_status HTTP状态码
         */
        DMResult(bool success = false, const std::string &user_id = "",
                 const std::string &message = "", const std::string &error_msg = "",
                 int http_status = 0)
            : success(success), user_id(user_id), message(message),
              error_msg(error_msg), http_status(http_status) {
        }
    };

    /**
 * @brief 批量私信发送结果统计结构体
 */
    struct BatchDMResult {
        int success_count; // 成功发送的数量
        int failure_count; // 发送失败的数量
        std::vector<DMResult> results; // 每个私信的详细发送结果

        /**
         * @brief 构造函数
         * @param success_count 成功数量
         * @param failure_count 失败数量
         * @param results 详细结果列表
         */
        BatchDMResult(int success_count = 0, int failure_count = 0, const std::vector<DMResult> &results = {})
            : success_count(success_count), failure_count(failure_count), results(results) {
        }
    };

    /**
     * @brief Twitter私信批量并发发送类
     *
     * 该类提供了Twitter私信的单条发送和批量并发发送功能
     * 使用cpr库进行网络请求，支持MultiPerform批量并发处理
     */
    class Twitter {
        std::string cookies_; // 账号cookies
        std::shared_ptr<spdlog::logger> logger_; // 日志记录器
        std::string proxy_url_; // 代理服务器URL（可选）

        /**
         * @brief 从cookies中提取必要的认证信息
         * @return 是否成功提取认证信息
         */
        bool extractAuthInfo();

        /**
         * @brief 构建私信发送的请求头
         * @return cpr::Header 请求头对象
         */
        cpr::Header buildHeaders() const;

        /**
         * @brief 构建私信发送的请求体
         * @param user_id 目标用户ID
         * @param message 消息内容
         * @return nlohmann::json 请求体JSON对象
         */
        nlohmann::json buildRequestBody(const std::string &user_id, const std::string &message) const;

        /**
         * @brief 解析API响应结果
         * @param response cpr响应对象
         * @param user_id 目标用户ID
         * @param message 发送的消息
         * @return DMResult 发送结果
         */
        DMResult parseResponse(const cpr::Response &response, const std::string &user_id,
                               const std::string &message) const;

        /**
         * @brief 从cookies中获取当前用户ID
         * @return std::string 当前用户ID
         */
        std::string getUserId() const;

        /**
         * @brief 获取客户端UUID
         * @return std::string 客户端UUID
         */
        static std::string getClientUuid();

        /**
         * @brief 从cookies中获取CSRF token
         * @return std::string CSRF token
         */
        std::string getCsrfToken() const;

        /**
         * @brief 获取客户端事务ID
         * @return std::string 客户端事务ID
         */
        std::string getClientTransactionId() const;

    public:
        /**
         * @brief 构造函数
         * @param cookies 账号cookies字符串
         * @param proxy_url 代理服务器URL（可选，格式：http://host:port 或 socks5://host:port）
         * @throws std::invalid_argument 当cookies格式无效时抛出异常
         */
        explicit Twitter(std::string cookies, std::string proxy_url = "");

        /**
         * @brief 析构函数
         */
        ~Twitter() = default;

        // 禁用拷贝构造和赋值操作
        Twitter(const Twitter &) = delete;

        Twitter &operator=(const Twitter &) = delete;

        // 允许移动构造和赋值操作
        Twitter(Twitter &&) = default;

        Twitter &operator=(Twitter &&) = default;

        /**
         * @brief 发送单条私信
         * @param user_id 目标用户ID
         * @param message 消息内容
         * @return DMResult 发送结果，包含成功状态和错误信息
         * @throws std::invalid_argument 当参数无效时抛出异常
         * @throws std::runtime_error 当网络请求失败时抛出异常
         */
        DMResult sendDirectMessage(const std::string &user_id, const std::string &message);

        /**
         * @brief 批量发送私信（并发执行）
         * @param user_ids 目标用户ID列表
         * @param message 消息内容
         * @return std::vector<DMResult> 所有发送结果的列表
         * @throws std::invalid_argument 当参数无效时抛出异常
         * @throws std::runtime_error 当批量请求失败时抛出异常
         */
        BatchDMResult sendBatchDirectMessages(const std::vector<std::string> &user_ids,
                                              const std::string &message);

        /**
         * @brief 设置日志级别
         * @param level spdlog日志级别
         */
        void setLogLevel(spdlog::level::level_enum level);

        /**
         * @brief 获取当前cookies
         * @return const std::string& cookies字符串的常量引用
         */
        const std::string &getCookies() const { return cookies_; }

        /**
         * @brief 验证cookies是否有效
         * @return bool 是否有效
         */
        bool validateCookies() const;
    };
} // namespace twitter_dm

#endif // TWITTER_DM_H
