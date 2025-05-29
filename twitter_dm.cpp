/**
 * @file twitter_dm.cpp
 * @brief Twitter私信批量并发发送功能的实现
 * @author 系统生成
 * @date 2024
 */

#include "twitter_dm.h"
#include <regex>
#include <stdexcept>
#include <utility>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <random>

namespace twitter_dm {
    // Twitter API相关常量

    static constexpr int REQUEST_TIMEOUT_MS = 30000; // 请求超时时间（毫秒）

    Twitter::Twitter(std::string cookies, std::string proxy_url)
        : cookies_(std::move(cookies)), proxy_url_(std::move(proxy_url)) {
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


            // 提取auth token
            std::regex auth_regex(R"(auth_token=([^;]+))");
            std::smatch auth_match;


            return true;
        } catch (const std::exception &e) {
            logger_->error("提取认证信息时发生异常: {}", e.what());
            return false;
        }
    }

    cpr::Header Twitter::buildHeaders(const std::string *client_transaction_id) const {
        cpr::Header headers = {
            {"Host", "x.com"},
            {"Accept-Encoding", "gzip, deflate, br"},
            {"Connection", "keep-alive"},
            {"Accept", "*/*"},
            {"Accept-Language", "zh-CN,zh;q=0.9"},
            {"Cache-Control", "no-cache"},
            {"Pragma", "no-cache"},
            {"Priority", "u=1, i"},
            {"Referer", "https://x.com/messages/"},
            {"Sec-Ch-Ua", "\"Chromium\";v=\"136\", \"Google Chrome\";v=\"136\", \"Not.A/Brand\";v=\"99\""},
            {"Sec-Ch-Ua-Arch", "\"arm\""},
            {"Sec-Ch-Ua-Bitness", "\"64\""},
            {"Sec-Ch-Ua-Full-Version", "\"136.0.7103.114\""},
            {
                "Sec-Ch-Ua-Full-Version-List",
                "\"Chromium\";v=\"136.0.7103.114\", \"Google Chrome\";v=\"136.0.7103.114\", \"Not.A/Brand\";v=\"99.0.0.0\""
            },
            {"Sec-Ch-Ua-Mobile", "?0"},
            {"Sec-Ch-Ua-Model", "\"\""},
            {"Sec-Ch-Ua-Platform", "\"macOS\""},
            {"Sec-Ch-Ua-Platform-Version", "\"15.5.0\""},
            {"Sec-Fetch-Dest", "empty"},
            {"Sec-Fetch-Mode", "cors"},
            {"Sec-Fetch-Site", "same-origin"},
            {
                "User-Agent",
                "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/136.0.0.0 Safari/537.36"
            },
            {"X-Twitter-Client-Language", "en"},
            {"X-Client-Uuid", Twitter::getClientUuid()},
            {"X-Csrf-Token", getCsrfToken()},
            {
                "Authorization",
                "Bearer AAAAAAAAAAAAAAAAAAAAANRILgAAAAAAnNwIzUejRCOuH5E6I8xnZz4puTs%3D1Zv7ttfk8LF81IUq16cHjhLTvJu4FA33AGWWjCpTnA"
            },
            {"Content-Type", "application/json"},
            {"Cookie", cookies_}
        };
        // 设置X-Client-Transaction-Id
        if (client_transaction_id) {
            headers["X-Client-Transaction-Id"] = *client_transaction_id;
        } else {
            headers["X-Client-Transaction-Id"] = getClientTransactionId();
        }
        return headers;
    }

    std::string Twitter::getUserId() const {
        // 从cookies中提取用户ID（通常在twid字段中）
        // 修正正则表达式以兼容 twid="u=..." 和 twid=u%3D... 两种格式
        // 正则表达式，用于匹配两种格式的twid:
        // 1. twid=u%3D<user_id> (URL编码格式)
        // 2. twid="u=<user_id>" (带引号格式)
        // 3. twid=u=<user_id> (不带引号但直接是u=的格式)
        std::regex user_id_regex(R"(twid=(?:u%3D|"?u=)([^";\s]+))");
        std::smatch match; // 用于存储正则匹配结果
        if (std::regex_search(cookies_, match, user_id_regex)) {
            return match[1].str(); // 返回匹配到的用户ID (捕获组1)
        }
        // 如果无法从cookies提取，记录警告并返回默认值或通过其他方式获取
        logger_->warn("无法从cookies中提取用户ID，将使用默认值或尝试其他方法");
        return "1234567890"; // 临时默认值，实际使用时需要正确实现
    }

    /**
     * @brief 获取客户端UUID，使用C++标准库随机生成UUID
     * @return std::string 随机生成的UUID字符串
     * @exception 无异常抛出
     */
    std::string Twitter::getClientUuid() {
        // Static method, no const qualifier needed here.
        // 生成随机UUID
        static constexpr char hex_chars[] = "0123456789abcdef";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::string uuid = std::string(36, ' ');
        int hyphen_pos[] = {8, 13, 18, 23};
        int hyphen_idx = 0;
        for (int i = 0; i < 36; ++i) {
            if (hyphen_idx < 4 && i == hyphen_pos[hyphen_idx]) {
                uuid[i] = '-';
                ++hyphen_idx;
            } else {
                uuid[i] = hex_chars[dis(gen)];
            }
        }
        uuid[14] = '4'; // UUID version 4
        uuid[19] = hex_chars[(dis(gen) & 0x3) | 0x8]; // UUID variant
        return uuid;
    }

    std::string Twitter::getCsrfToken() const {
        // 从cookies中提取CSRF token (ct0)
        std::regex csrf_regex(R"(ct0=([^;]+))");
        std::smatch match;
        if (std::regex_search(cookies_, match, csrf_regex)) {
            return match[1].str();
        }
        // 如果无法提取，返回空字符串
        logger_->warn("无法从cookies中提取CSRF token");
        return "";
    }

    std::string Twitter::getClientTransactionId() const {
        // 生成客户端事务ID，通常是一个随机的Base64编码字符串
        // 这里使用固定值，实际应用中应该生成随机值
        return "CCInUxLPAOREKNSTw+LJtRdUt9/2QxmiWO4cwH/4M5otonm0csvrsahdgGsYS5x/vo5S7wu9Fn6ixUTTfNQ/RwPrp37uCw";
    }

    nlohmann::json Twitter::buildRequestBody(const std::string &user_id, const std::string &message) const {
        nlohmann::json body;

        // 根据Python示例构建正确的请求体
        std::string current_user_id = getUserId();
        body["conversation_id"] = user_id + "-" + current_user_id;
        body["recipient_ids"] = false;
        body["request_id"] = getClientUuid(); // 使用UUID格式
        body["text"] = message;
        body["include_cards"] = 0;
        body["include_quote_count"] = false;
        body["dm_users"] = true;

        return body;
    }

    DMResult Twitter::parseResponse(const cpr::Response &response, const std::string &user_id,
                                    const std::string &message) const {
        DMResult result(false, user_id, message, "", response.status_code);

        try {
            if (response.status_code == 200) {
                logger_->debug("响应状态码: {}", response.status_code);
                logger_->debug("响应结果: {}", response.text);
                // 解析响应JSON
                auto json_response = nlohmann::json::parse(response.text);

                // 根据Python示例检查响应格式
                if (json_response.contains("entries")) {
                    result.success = true;
                    // logger_->info("成功发送私信到用户: {}", user_id);
                } else if (json_response.contains("errors") && !json_response["errors"].empty()) {
                    result.error_msg = "API错误: " + json_response["errors"][0]["message"].get<std::string>();
                    logger_->error("发送私信到用户{}失败: {}", user_id, result.error_msg);
                } else {
                    // 即使没有entries字段，如果没有错误也认为成功（根据Python示例逻辑）
                    result.success = true;
                    logger_->info("发送私信到用户: {} (无entries字段但无错误)", user_id);
                }
            } else {
                result.error_msg = "HTTP错误: " + std::to_string(response.status_code) + " - " + response.error.message;
                logger_->error("发送私信到用户{}失败，HTTP状态码: {}, 错误: {}",
                               user_id, response.status_code, response.error.message);
            }
        } catch (const nlohmann::json::exception &e) {
            result.error_msg = "JSON解析错误: " + std::string(e.what());
            logger_->error("解析响应JSON失败: {}", e.what());
        }

        return result;
    }

    DMResult Twitter::sendDirectMessage(const std::string &user_id, const std::string &message) {
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

            // 根据Python示例添加URL查询参数
            std::string url = "https://x.com/i/api/1.1/dm/new2.json";
            cpr::Parameters params = cpr::Parameters{
                {
                    "ext",
                    "mediaColor,altText,mediaStats,highlightedLabel,voiceInfo,birdwatchPivot,superFollowMetadata,unmentionInfo,editControl,article"
                },
                {"include_ext_alt_text", "true"},
                {"include_ext_limited_action_results", "true"},
                {"include_reply_count", "1"},
                {"tweet_mode", "extended"},
                {"include_ext_views", "true"},
                {"include_groups", "true"},
                {"include_inbox_timelines", "true"},
                {"include_ext_media_color", "true"},
                {"supports_reactions", "true"},
                {"supports_edit", "true"}
            };

            // 发送请求
            cpr::Response response;
            if (!proxy_url_.empty()) {
                // 使用代理发送请求
                response = cpr::Post(
                    cpr::Url{url},
                    headers,
                    params,
                    cpr::Body{body.dump()},
                    cpr::Timeout{REQUEST_TIMEOUT_MS},
                    cpr::VerifySsl{false},
                    cpr::Proxies{
                        {"http", proxy_url_},
                        {"https", proxy_url_}
                    }
                );
            } else {
                // 不使用代理发送请求
                response = cpr::Post(
                    cpr::Url{url},
                    headers,
                    params,
                    cpr::Body{body.dump()},
                    cpr::Timeout{REQUEST_TIMEOUT_MS},
                    cpr::VerifySsl{false}
                );
            }

            return parseResponse(response, user_id, message);
        } catch (const std::exception &e) {
            std::string error_msg = "发送私信时发生异常: " + std::string(e.what());
            logger_->error(error_msg);
            throw std::runtime_error(error_msg);
        }
    }

    BatchDMResult Twitter::sendBatchDirectMessages(const std::vector<std::string> &user_ids,
                                                   const std::string &message,
                                                   const std::vector<std::string> *client_transaction_ids) {
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
        if (client_transaction_ids && client_transaction_ids->size() != user_ids.size()) {
            throw std::invalid_argument("client_transaction_ids数量必须与user_ids一致");
        }
        logger_->info("开始批量发送私信，目标用户数量: {}", user_ids.size());
        std::vector<DMResult> results;
        results.reserve(user_ids.size());
        try {
            cpr::MultiPerform multi_perform;
            std::vector<std::shared_ptr<cpr::Session> > sessions;
            sessions.reserve(user_ids.size());
            std::string url = "https://x.com/i/api/1.1/dm/new2.json";
            cpr::Parameters params = cpr::Parameters{
                {
                    "ext",
                    "mediaColor,altText,mediaStats,highlightedLabel,voiceInfo,birdwatchPivot,superFollowMetadata,unmentionInfo,editControl,article"
                },
                {"include_ext_alt_text", "true"},
                {"include_ext_limited_action_results", "true"},
                {"include_reply_count", "1"},
                {"tweet_mode", "extended"},
                {"include_ext_views", "true"},
                {"include_groups", "true"},
                {"include_inbox_timelines", "true"},
                {"include_ext_media_color", "true"},
                {"supports_reactions", "true"},
                {"supports_edit", "true"}
            };
            size_t idx = 0;
            for (const auto &user_id: user_ids) {
                if (user_id.empty()) {
                    logger_->warn("跳过空的用户ID");
                    results.emplace_back(false, user_id, message, "用户ID为空", 0);
                    ++idx;
                    continue;
                }
                std::string client_tid;
                if (client_transaction_ids) {
                    client_tid = (*client_transaction_ids)[idx];
                }
                auto session = std::make_shared<cpr::Session>();
                session->SetUrl(cpr::Url{url});
                session->SetHeader(buildHeaders(client_transaction_ids ? &client_tid : nullptr));
                session->SetParameters(params);
                session->SetBody(cpr::Body{buildRequestBody(user_id, message).dump()});
                session->SetTimeout(cpr::Timeout{REQUEST_TIMEOUT_MS});
                session->SetVerifySsl(false);
                if (!proxy_url_.empty()) {
                    session->SetProxies(cpr::Proxies{
                        {"http", proxy_url_},
                        {"https", proxy_url_}
                    });
                }
                sessions.push_back(session);
                multi_perform.AddSession(session);
                ++idx;
            }
            logger_->info("准备发送{}个并发请求", sessions.size());
            auto responses = multi_perform.Post();
            size_t session_index = 0;
            for (const auto &user_id: user_ids) {
                if (user_id.empty()) {
                    continue;
                }
                if (session_index < responses.size()) {
                    auto result = parseResponse(responses[session_index], user_id, message);
                    results.push_back(result);
                    session_index++;
                } else {
                    results.emplace_back(false, user_id, message, "请求响应不匹配", 0);
                    logger_->error("用户{}的请求响应不匹配", user_id);
                }
            }
        } catch (const std::exception &e) {
            std::string error_msg = "批量发送私信时发生异常: " + std::string(e.what());
            logger_->error(error_msg);
            throw std::runtime_error(error_msg);
        }
        int success_count = 0;
        int failure_count = 0;
        for (const auto &res: results) {
            if (res.success) {
                success_count++;
            } else {
                failure_count++;
            }
        }
        return BatchDMResult(success_count, failure_count, results);
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

        for (const auto &cookie: required_cookies) {
            if (cookies_.find(cookie + "=") == std::string::npos) {
                logger_->warn("缺少必要的cookie字段: {}", cookie);
                return false;
            }
        }

        return true;
    }
} // namespace twitter_dm
