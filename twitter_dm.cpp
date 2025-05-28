/**
 * @file twitter_dm.cpp
 * @brief Twitter私信批量并发发送功能的实现
 * @author 系统生成
 * @date 2024
 */

#include "twitter_dm.h"
#include <regex>
#include <thread>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace twitter_dm {

// Twitter API相关常量
static const std::string TWITTER_API_BASE = "https://twitter.com/i/api/1.1";
static const std::string DM_SEND_ENDPOINT = "/dm/new2.json";
static const int MAX_CONCURRENT_REQUESTS = 10; // 最大并发请求数
static const int REQUEST_TIMEOUT_MS = 30000;   // 请求超时时间（毫秒）

Twitter::Twitter(const std::string& cookies) 
    : cookies_(cookies), base_url_(TWITTER_API_BASE) {
    
    // 初始化日志记录器
    logger_ = spdlog::stdout_color_mt("twitter_dm");
    logger_->set_level(spdlog::level::info);
    
    // 验证cookies格式
    if (cookies_.empty()) {
        throw std::invalid_argument("Cookies不能为空");
    }
    
    // 提取认证信息
    if (!extractAuthInfo()) {
        throw std::invalid_argument("无法从cookies中提取有效的认证信息");
    }
    
    logger_->info("Twitter DM客户端初始化成功");
}

bool Twitter::extractAuthInfo() {
    try {
        // 提取CSRF token
        std::regex csrf_regex(R"(ct0=([^;]+))");
        std::smatch csrf_match;
        if (std::regex_search(cookies_, csrf_match, csrf_regex)) {
            csrf_token_ = csrf_match[1].str();
        } else {
            logger_->error("无法从cookies中找到CSRF token (ct0)");
            return false;
        }
        
        // 提取auth token
        std::regex auth_regex(R"(auth_token=([^;]+))");
        std::smatch auth_match;
        if (std::regex_search(cookies_, auth_match, auth_regex)) {
            auth_token_ = auth_match[1].str();
        } else {
            logger_->error("无法从cookies中找到auth token");
            return false;
        }
        
        logger_->debug("成功提取认证信息: CSRF={}, Auth={}", 
                      csrf_token_.substr(0, 8) + "...", 
                      auth_token_.substr(0, 8) + "...");
        return true;
        
    } catch (const std::exception& e) {
        logger_->error("提取认证信息时发生异常: {}", e.what());
        return false;
    }
}

cpr::Header Twitter::buildHeaders() const {
    return cpr::Header{
        {"Accept", "*/*"},
        {"Accept-Language", "en-US,en;q=0.9"},
        {"Authorization", "Bearer AAAAAAAAAAAAAAAAAAAAANRILgAAAAAAnNwIzUejRCOuH5E6I8xnZz4puTs%3D1Zv7ttfk8LF81IUq16cHjhLTvJu4FA33AGWWjCpTnA"},
        {"Content-Type", "application/json"},
        {"Cookie", cookies_},
        {"Referer", "https://twitter.com/messages"},
        {"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"},
        {"X-Csrf-Token", csrf_token_},
        {"X-Twitter-Active-User", "yes"},
        {"X-Twitter-Auth-Type", "OAuth2Session"},
        {"X-Twitter-Client-Language", "en"}
    };
}

nlohmann::json Twitter::buildRequestBody(const std::string& user_id, const std::string& message) const {
    nlohmann::json body;
    
    // 构建请求体
    body["recipient_id"] = user_id;
    body["text"] = message;
    body["media_id"] = "";
    
    // 添加必要的Twitter API参数
    body["request_id"] = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    
    return body;
}

DMResult Twitter::parseResponse(const cpr::Response& response, const std::string& user_id, const std::string& message) const {
    DMResult result(false, user_id, message, "", response.status_code);
    
    try {
        if (response.status_code == 200) {
            // 解析响应JSON
            auto json_response = nlohmann::json::parse(response.text);
            
            // 检查是否有错误
            if (json_response.contains("errors") && !json_response["errors"].empty()) {
                result.error_msg = "API错误: " + json_response["errors"][0]["message"].get<std::string>();
                logger_->warn("发送私信到用户{}失败: {}", user_id, result.error_msg);
            } else {
                result.success = true;
                logger_->info("成功发送私信到用户: {}", user_id);
            }
        } else {
            result.error_msg = "HTTP错误: " + std::to_string(response.status_code) + " - " + response.error.message;
            logger_->error("发送私信到用户{}失败，HTTP状态码: {}, 错误: {}", 
                          user_id, response.status_code, response.error.message);
        }
    } catch (const nlohmann::json::exception& e) {
        result.error_msg = "JSON解析错误: " + std::string(e.what());
        logger_->error("解析响应JSON失败: {}", e.what());
    }
    
    return result;
}

DMResult Twitter::sendDirectMessage(const std::string& user_id, const std::string& message) {
    // 参数验证
    if (user_id.empty()) {
        throw std::invalid_argument("用户ID不能为空");
    }
    if (message.empty()) {
        throw std::invalid_argument("消息内容不能为空");
    }
    if (message.length() > 10000) {
        throw std::invalid_argument("消息内容过长，最大支持10000字符");
    }
    
    logger_->info("开始发送私信到用户: {}", user_id);
    
    try {
        // 构建请求
        auto headers = buildHeaders();
        auto body = buildRequestBody(user_id, message);
        std::string url = base_url_ + DM_SEND_ENDPOINT;
        
        // 发送请求
        auto response = cpr::Post(
            cpr::Url{url},
            headers,
            cpr::Body{body.dump()},
            cpr::Timeout{REQUEST_TIMEOUT_MS}
        );
        
        return parseResponse(response, user_id, message);
        
    } catch (const std::exception& e) {
        std::string error_msg = "发送私信时发生异常: " + std::string(e.what());
        logger_->error(error_msg);
        throw std::runtime_error(error_msg);
    }
}

std::vector<DMResult> Twitter::sendBatchDirectMessages(const std::vector<std::string>& user_ids, const std::string& message) {
    // 参数验证
    if (user_ids.empty()) {
        throw std::invalid_argument("用户ID列表不能为空");
    }
    if (message.empty()) {
        throw std::invalid_argument("消息内容不能为空");
    }
    if (message.length() > 10000) {
        throw std::invalid_argument("消息内容过长，最大支持10000字符");
    }
    
    logger_->info("开始批量发送私信，目标用户数量: {}", user_ids.size());
    
    std::vector<DMResult> results;
    results.reserve(user_ids.size());
    
    try {
        // 使用cpr::MultiPerform进行批量并发请求
        cpr::MultiPerform multi_perform;
        std::vector<std::shared_ptr<cpr::Session>> sessions;
        sessions.reserve(user_ids.size());
        
        // 构建所有请求会话
        auto headers = buildHeaders();
        std::string url = base_url_ + DM_SEND_ENDPOINT;
        
        for (const auto& user_id : user_ids) {
            if (user_id.empty()) {
                logger_->warn("跳过空的用户ID");
                results.emplace_back(false, user_id, message, "用户ID为空", 0);
                continue;
            }
            
            auto session = std::make_shared<cpr::Session>();
            session->SetUrl(cpr::Url{url});
            session->SetHeader(headers);
            session->SetBody(cpr::Body{buildRequestBody(user_id, message).dump()});
            session->SetTimeout(cpr::Timeout{REQUEST_TIMEOUT_MS});
            
            sessions.push_back(session);
            multi_perform.AddSession(session);
        }
        
        logger_->info("准备发送{}个并发请求", sessions.size());
        
        // 执行批量请求
        auto responses = multi_perform.Get();
        
        // 处理响应结果
        size_t session_index = 0;
        for (size_t i = 0; i < user_ids.size(); ++i) {
            const auto& user_id = user_ids[i];
            
            if (user_id.empty()) {
                // 跳过空用户ID，结果已经在上面添加了
                continue;
            }
            
            if (session_index < responses.size()) {
                auto result = parseResponse(responses[session_index], user_id, message);
                results.push_back(result);
                session_index++;
            } else {
                // 如果响应数量不匹配，添加错误结果
                results.emplace_back(false, user_id, message, "请求响应不匹配", 0);
                logger_->error("用户{}的请求响应不匹配", user_id);
            }
        }
        
        // 统计结果
        size_t success_count = std::count_if(results.begin(), results.end(), 
                                           [](const DMResult& r) { return r.success; });
        
        logger_->info("批量发送完成，成功: {}/{}", success_count, results.size());
        
    } catch (const std::exception& e) {
        std::string error_msg = "批量发送私信时发生异常: " + std::string(e.what());
        logger_->error(error_msg);
        throw std::runtime_error(error_msg);
    }
    
    return results;
}

void Twitter::setLogLevel(spdlog::level::level_enum level) {
    if (logger_) {
        logger_->set_level(level);
        logger_->info("日志级别已设置为: {}", spdlog::level::to_string_view(level));
    }
}

bool Twitter::validateCookies() const {
    // 基本格式验证
    if (cookies_.empty()) {
        return false;
    }
    
    // 检查必要的cookie字段
    std::vector<std::string> required_cookies = {"ct0", "auth_token"};
    
    for (const auto& cookie : required_cookies) {
        if (cookies_.find(cookie + "=") == std::string::npos) {
            logger_->warn("缺少必要的cookie字段: {}", cookie);
            return false;
        }
    }
    
    return true;
}

} // namespace twitter_dm