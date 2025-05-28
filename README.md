# Twitter DM Static Library

一个用于Twitter私信批量并发发送的C++静态库。

## 项目功能

本项目实现了Twitter私信的批量并发发送功能，支持：

- 🚀 **单条私信发送**: 向指定用户发送单条私信
- 📦 **批量并发发送**: 同时向多个用户发送私信，提高效率
- 🔒 **安全认证**: 基于cookies的Twitter认证机制
- 📝 **详细日志**: 完整的发送日志和错误追踪
- ⚡ **高性能**: 使用cpr::MultiPerform实现真正的并发请求

## 技术栈

### 核心库
- **cpr**: 现代C++ HTTP客户端库，用于网络请求
- **cpr::MultiPerform**: 实现批量并发HTTP请求

### 依赖库
- **spdlog** (>= 1.8.0): 高性能日志库
- **Google Test** (>= 1.11.0): 单元测试框架
- **nlohmann/json** (>= 3.11.0): JSON解析库
- **libcpr** (>= 1.10.0): HTTP请求库

## 项目结构

```
twitter-dm-static/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明文档
├── library.h               # 主要头文件（兼容性）
├── library.cpp             # 主要实现文件
├── twitter_dm.h            # Twitter类头文件
├── twitter_dm.cpp          # Twitter类实现文件
├── example.cpp             # 使用示例
└── cmake-build-debug/      # 构建输出目录
```

## 快速开始

### 1. 安装依赖

在macOS上使用Homebrew安装依赖：

```bash
# 安装基础依赖
brew install cmake pkg-config

# 安装项目依赖
brew install spdlog nlohmann-json cpr googletest
```

### 2. 构建项目

```bash
# 克隆项目
cd /path/to/your/project

# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake ..

# 编译
make
```

### 3. 基本使用

```cpp
#include "twitter_dm.h"
#include <iostream>

int main() {
    try {
        // 初始化Twitter客户端（需要有效的cookies）
        std::string cookies = "ct0=your_csrf_token; auth_token=your_auth_token; ...";
        twitter_dm::Twitter client(cookies);
        
        // 发送单条私信
        auto result = client.sendDirectMessage("123456789", "Hello, World!");
        if (result.success) {
            std::cout << "私信发送成功!" << std::endl;
        }
        
        // 批量发送私信
        std::vector<std::string> user_ids = {"123456789", "987654321"};
        auto results = client.sendBatchDirectMessages(user_ids, "批量消息");
        
        for (const auto& res : results) {
            if (res.success) {
                std::cout << "用户 " << res.user_id << " 发送成功" << std::endl;
            } else {
                std::cout << "用户 " << res.user_id << " 发送失败: " << res.error_msg << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    
    return 0;
}
```

## API 文档

### Twitter 类

#### 构造函数

```cpp
Twitter(const std::string& cookies)
```

**参数:**
- `cookies`: Twitter账号的cookies字符串，必须包含`ct0`和`auth_token`

**异常:**
- `std::invalid_argument`: 当cookies格式无效时抛出

#### 发送单条私信

```cpp
DMResult sendDirectMessage(const std::string& user_id, const std::string& message)
```

**参数:**
- `user_id`: 目标用户的Twitter ID
- `message`: 要发送的消息内容（最大10000字符）

**返回值:**
- `DMResult`: 包含发送结果的结构体

**异常:**
- `std::invalid_argument`: 参数无效时抛出
- `std::runtime_error`: 网络请求失败时抛出

#### 批量发送私信

```cpp
std::vector<DMResult> sendBatchDirectMessages(const std::vector<std::string>& user_ids, const std::string& message)
```

**参数:**
- `user_ids`: 目标用户ID列表
- `message`: 要发送的消息内容

**返回值:**
- `std::vector<DMResult>`: 所有发送结果的列表

### DMResult 结构体

```cpp
struct DMResult {
    bool success;           // 发送是否成功
    std::string user_id;    // 目标用户ID
    std::string message;    // 发送的消息内容
    std::string error_msg;  // 错误信息（如果有）
    int http_status;        // HTTP状态码
};
```

## 获取Twitter Cookies

1. 在浏览器中登录Twitter
2. 打开开发者工具（F12）
3. 转到Network标签页
4. 发送一条私信
5. 在请求头中找到Cookie字段
6. 复制完整的Cookie值

**重要**: 请确保cookies包含以下必要字段：
- `ct0`: CSRF令牌
- `auth_token`: 认证令牌

## 注意事项

### 安全性
- 🔐 **保护cookies**: 不要在代码中硬编码cookies，使用环境变量或配置文件
- 🚫 **避免滥用**: 遵守Twitter的使用条款，避免发送垃圾信息
- ⏱️ **请求频率**: 注意控制请求频率，避免触发反垃圾机制

### 性能优化
- 📊 **并发控制**: 默认最大并发数为10，可根据需要调整
- ⏰ **超时设置**: 默认请求超时30秒
- 📝 **日志级别**: 生产环境建议设置为info或warn级别

### 错误处理
- ✅ **参数验证**: 所有输入参数都会进行验证
- 🔍 **详细错误信息**: 提供具体的错误原因和HTTP状态码
- 📋 **日志记录**: 完整的操作日志便于调试

## 示例程序

运行示例程序：

```bash
# 编译示例（如果包含在CMakeLists.txt中）
g++ -std=c++20 example.cpp -ltwitter_dm_static -lcpr -lspdlog -o example

# 运行示例
./example
```

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进这个项目！

## 更新日志

### v1.0.0
- ✨ 初始版本发布
- 🚀 支持单条和批量私信发送
- 📦 完整的CMake构建支持
- 📝 详细的文档和示例