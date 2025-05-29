# Twitter DM Static Library

一个用于Twitter私信批量并发发送的C++核心库，通过pybind11提供Python接口。

## 项目功能

本项目实现了Twitter私信的批量并发发送功能，支持：

- 🚀 **单条私信发送**: 向指定用户发送单条私信
- 📦 **批量并发发送**: 同时向多个用户发送私信，提高效率
- 🔒 **安全认证**: 基于cookies的Twitter认证机制
- 📝 **详细日志**: 完整的发送日志和错误追踪
- ⚡ **高性能**: 使用cpr::MultiPerform实现真正的并发请求

## 构建环境

- **操作系统**: macOS, Linux (在其他Unix-like系统上也可能工作，但主要在 macOS 和主流 Linux 发行版上测试)
- **C++编译器**: 支持C++17标准的编译器 (例如 Clang, GCC)
- **CMake**: >= 3.10
- **Python**: 用于pybind11绑定 (建议使用Python 3.x)

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

本项目使用CMake的`FetchContent`来管理大部分C++依赖（如spdlog, nlohmann-json, cpr, googletest, pybind11），因此通常不需要手动安装这些库。

您需要确保您的系统已安装：

- **CMake**: 版本 >= 3.10
- **C++编译器**: 支持C++17，例如Apple Clang (Xcode Command Line Tools的一部分) 或 GCC。
- **Python 3**: 用于构建Python绑定。确保`python3`命令可用，并且相关的开发头文件已安装（通常随Python一同安装）。
- **Git**: `FetchContent` 需要git来下载依赖。

在macOS上，可以通过Homebrew安装必要的工具：

```bash
# 安装构建工具
brew install cmake git

# 如果尚未安装Xcode Command Line Tools (包含Clang编译器)
# xcode-select --install

# Python 3 通常已预装在较新的macOS版本中，或者可以通过Homebrew安装
# brew install python3 
```

在Linux (例如 Ubuntu/Debian)上，可以使用apt：

```bash
# 安装构建工具和依赖
sudo apt update
sudo apt install cmake g++ python3-dev git

# 确保安装了 C++17 兼容的 GCC/Clang
# 对于 python3-dev，它提供了构建 Python C 扩展所需的头文件和静态库
```

在其他Linux发行版上，请使用相应的包管理器 (如 `yum`, `dnf`, `pacman` 等) 安装 `cmake`、`gcc` (或 `clang`，确保支持C++17)、`python3-devel` (或等效包名) 和 `git`。

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

### 3. 基本使用 (Python)

构建完成后，会在构建目录的 `python_example` (或类似名称，取决于您的 `CMakeLists.txt` 配置)下生成一个Python模块 (例如 `twitter_dm.cpython-39-darwin.so`)。您可以将其导入到Python脚本中使用。

```python
import twitter_dm # 假设 .so 文件在 PYTHONPATH 中或者与脚本在同一目录
import asyncio

def main():
    try:
        # 初始化Twitter客户端（需要有效的cookies）
        cookies = "ct0=your_csrf_token; auth_token=your_auth_token; ..."
        client = twitter_dm.Twitter(cookies)
        
        # 发送单条私信
        result = client.send_direct_message("123456789", "Hello from Python!")
        if result.success:
            print(f"私信发送成功! Event ID: {result.event_id}")
        else:
            print(f"私信发送失败: {result.error_msg}")

        # 准备批量发送的用户ID列表
        user_ids = ["user_id_1", "user_id_2", "user_id_3"]
        message_content = "这是一条来自Python的批量测试消息！"

        # 批量发送私信 (同步版本)
        # print("\n开始同步批量发送...")
        # batch_results_sync = client.send_batch_direct_messages(user_ids, message_content)
        # for res_sync in batch_results_sync:
        #     if res_sync.success:
        #         print(f"用户 {res_sync.user_id} (同步) 发送成功. Event ID: {res_sync.event_id}")
        #     else:
        #         print(f"用户 {res_sync.user_id} (同步) 发送失败: {res_sync.error_msg}")

        # 批量发送私信 (异步版本)
        print("\n开始异步批量发送...")
        # 注意：Python侧的异步调用需要C++侧有相应的异步接口暴露
        # 以下为调用C++同步批量发送接口的示例，如果需要Python端的真异步，
        # C++的send_batch_direct_messages_async需要返回一个可以被Python await的对象，
        # 或者在Python端使用线程池等方式包装同步调用。
        # 假设 client.send_batch_direct_messages_async 存在且设计为Python异步兼容
        # async_results = await client.send_batch_direct_messages_async(user_ids, message_content)
        # for res_async in async_results:
        #    if res_async.success:
        #        print(f"用户 {res_async.user_id} (异步) 发送成功. Event ID: {res_async.event_id}")
        #    else:
        #        print(f"用户 {res_async.user_id} (异步) 发送失败: {res_async.error_msg}")
        # 当前 C++ 库的 sendBatchDirectMessages 是同步阻塞的，若要在 Python 中实现并发，
        # 可以考虑使用 Python 的 `concurrent.futures.ThreadPoolExecutor` 来包装调用。

        print("\n使用 ThreadPoolExecutor 进行并发发送示例:")
        from concurrent.futures import ThreadPoolExecutor

        def send_message_wrapper(user_id):
            # 这里每次都重新创建client是为了演示，实际应用中应复用client实例
            # 或者确保client实例是线程安全的（当前C++实现可能不是线程安全的，需要注意）
            # temp_client = twitter_dm.Twitter(cookies) 
            # return temp_client.send_direct_message(user_id, message_content)
            # 假设 client 是线程安全的，或者在单线程中使用
            return client.send_direct_message(user_id, message_content)

        with ThreadPoolExecutor(max_workers=5) as executor:
            futures = [executor.submit(send_message_wrapper, uid) for uid in user_ids]
            for i, future in enumerate(futures):
                res = future.result()
                if res.success:
                    print(f"用户 {user_ids[i]} (并发) 发送成功. Event ID: {res.event_id}")
                else:
                    print(f"用户 {user_ids[i]} (并发) 发送失败: {res.error_msg}")

    except Exception as e:
        print(f"Python 端发生错误: {e}")

if __name__ == "__main__":
    main()

```

### 基本使用 (C++ - 如果您仍希望直接使用C++库)

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