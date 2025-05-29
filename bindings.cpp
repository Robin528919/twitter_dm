/**
 * @file bindings.cpp
 * @brief 使用pybind11为twitter_dm库创建Python绑定
 * @author 系统生成
 * @date 2024
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h> // 用于自动转换STL容器，如std::vector
#include "twitter_dm.h" // 引入C++库的头文件

namespace py = pybind11;

// PYBIND11_MODULE宏定义了Python模块的入口点
// 第一个参数 (twitter_dm_pybind) 是模块名，Python中通过 import twitter_dm_pybind 来导入
// 第二个参数 (m) 是 py::module_ 类型的变量，表示模块对象
PYBIND11_MODULE(twitter_dm, m) {
    // 可选的模块文档字符串
    m.doc() = "twitter_dm库的pybind11 Python绑定";

    // 绑定DMResult结构体
    py::class_<twitter_dm::DMResult>(m, "DMResult")
            // .def(py::init<>()) // 如果有默认构造函数，可以这样绑定
            // 绑定构造函数，需要指定参数类型
            .def(py::init<bool, const std::string &, const std::string &, const std::string &, int>(),
                 py::arg("success") = false, // 为参数指定默认值，使其在Python中可选
                 py::arg("user_id") = "",
                 py::arg("message") = "",
                 py::arg("error_msg") = "",
                 py::arg("http_status") = 0)
            // 绑定成员变量，使其在Python中可读写
            .def_readwrite("success", &twitter_dm::DMResult::success, "发送是否成功")
            .def_readwrite("user_id", &twitter_dm::DMResult::user_id, "目标用户ID")
            .def_readwrite("message", &twitter_dm::DMResult::message, "发送的消息内容")
            .def_readwrite("error_msg", &twitter_dm::DMResult::error_msg, "错误信息（如果有）")
            .def_readwrite("http_status", &twitter_dm::DMResult::http_status, "HTTP状态码")
            // 添加一个__repr__方法，方便在Python中打印对象信息
            .def("__repr__",
                 [](const twitter_dm::DMResult &r) {
                     return "<DMResult success=" + std::to_string(r.success) +
                            ", user_id='" + r.user_id + "'" +
                            ", message='" + r.message + "'" +
                            ", error_msg='" + r.error_msg + "'" +
                            ", http_status=" + std::to_string(r.http_status) + ">";
                 }
            );

    // 绑定BatchDMResult结构体
    py::class_<twitter_dm::BatchDMResult>(m, "BatchDMResult")
            .def(py::init<int, int, const std::vector<twitter_dm::DMResult> &>(),
                 py::arg("success_count") = 0,
                 py::arg("failure_count") = 0,
                 py::arg("results") = std::vector<twitter_dm::DMResult>{})
            .def_readwrite("success_count", &twitter_dm::BatchDMResult::success_count, "成功发送的数量")
            .def_readwrite("failure_count", &twitter_dm::BatchDMResult::failure_count, "发送失败的数量")
            .def_readwrite("results", &twitter_dm::BatchDMResult::results, "每个私信的详细发送结果")
            .def("__repr__",
                 [](const twitter_dm::BatchDMResult &br) {
                     return "<BatchDMResult success_count=" + std::to_string(br.success_count) +
                            ", failure_count=" + std::to_string(br.failure_count) +
                            ", results_count=" + std::to_string(br.results.size()) + ">";
                 }
            );

    // 绑定Twitter类
    py::class_<twitter_dm::Twitter>(m, "Twitter")
            // 绑定构造函数，允许Python通过 Twitter(cookies, proxy_url) 创建对象
            // py::arg用于为Python中的参数命名，并可以提供默认值
            .def(py::init<std::string, std::string>(),
                 py::arg("cookies"),
                 py::arg("proxy_url") = "", "Twitter账号的cookies和可选的代理URL")
            // 绑定类的成员函数
            // &twitter_dm::Twitter::sendDirectMessage 是指向成员函数的指针
            .def("send_direct_message", &twitter_dm::Twitter::sendDirectMessage,
                 py::arg("user_id"), py::arg("message"),
                 "发送单条私信")
            .def("send_batch_direct_messages", &twitter_dm::Twitter::sendBatchDirectMessages,
                 py::arg("user_ids"),
                 py::arg("message"),
                 py::arg("client_transaction_ids"), // 新增可选参数
                 "批量发送私信（并发执行），可选参数client_transaction_ids用于指定每个请求的X-Client-Transaction-Id")
            // 绑定setLogLevel方法，注意spdlog::level::level_enum可能需要特殊处理或转换为int
            // pybind11可以直接处理枚举类型，如果spdlog::level::level_enum已正确定义
            .def("set_log_level", &twitter_dm::Twitter::setLogLevel,
                 py::arg("level"),
                 "设置日志级别 (0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=critical, 6=off)")
            .def("get_cookies", &twitter_dm::Twitter::getCookies, "获取当前cookies")
            .def("validate_cookies", &twitter_dm::Twitter::validateCookies, "验证cookies是否有效");

    // 如果spdlog::level::level_enum没有直接被pybind11识别，可以绑定一个辅助枚举
    py::enum_<spdlog::level::level_enum>(m, "LogLevel", py::arithmetic())
            .value("trace", spdlog::level::trace)
            .value("debug", spdlog::level::debug)
            .value("info", spdlog::level::info)
            .value("warn", spdlog::level::warn)
            .value("error", spdlog::level::err) // 注意spdlog中error是err
            .value("critical", spdlog::level::critical)
            .value("off", spdlog::level::off)
            .export_values(); // 将枚举值导出到模块命名空间

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
