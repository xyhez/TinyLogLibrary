#pragma once

#include <memory>
#include <sstream>
#include <string>

#include "ConsoleSink.h"
#include "FileSink.h"
#include "Logger.h"

// CMake 通过 target_compile_definitions 注入项目根的绝对路径
// 没用 CMake 构建时退化为 "."（当前目录），保证总是能编过
#ifndef TINY_LOG_PROJECT_ROOT
#define TINY_LOG_PROJECT_ROOT "."
#endif

namespace logging {

    /**
     * @brief 获取默认 Logger（进程内单例）
     *
     * 必须加 inline，否则该定义会出现在每一个包含 Logging.h 的 TU 里，
     * 链接期会因 ODR 多重定义报错。
     */
    inline Logger* GetDefaultLogger() {
        static Logger default_logger;
        static bool initialized = false;
        if (!initialized) {
            default_logger.AddSink(std::make_shared<ConsoleSink>());
            // 用项目根拼出绝对路径，从任何 CWD 运行都写到同一个文件
            default_logger.AddSink(std::make_shared<FileSink>(
                std::string(TINY_LOG_PROJECT_ROOT) + "/logfile/log1.txt"));
            initialized = true;
        }
        return &default_logger;
    }

    namespace detail {

        /**
         * @brief 把 __FILE__/__LINE__/__func__ 拼成 "file:line func" 形式
         *
         * 真正的位置捕获是在调用方的宏展开点完成的，这里只负责字符串拼装。
         */
        inline std::string MakeLocation(const char* file, int line, const char* func) {
            std::ostringstream oss;
            oss << file << ":" << line << " " << func;
            return oss.str();
        }

        /**
         * @brief 宏的内部转发函数：把位置信息打包后交给 Logger
         *
         * 依赖 Logger 提供如下重载：
         *     void Print(LogLevel, const std::string& msg, const std::string& source_location);
         * 若该重载尚未实现，需要在 Logger.h / Logger.cpp 中补上。
         */
        inline void PrintWithLocation(LogLevel level,
                                      const std::string& msg,
                                      const char* file,
                                      int line,
                                      const char* func) {
            GetDefaultLogger()->Print(level, msg, MakeLocation(file, line, func));
        }

    } // namespace detail

} // namespace logging


// ============================================================
// 使用入口
// 用法：
//     LOG_INFO("hello");
//     LOG_ERROR("something broke: " + reason);
// ============================================================

#define LOG_TRACE(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Trace,    msg, __FILE__, __LINE__, __func__)

#define LOG_DEBUG(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Debug,    (msg), __FILE__, __LINE__, __func__)

#define LOG_INFO(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Info,     (msg), __FILE__, __LINE__, __func__)

#define LOG_WARN(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Warn,     (msg), __FILE__, __LINE__, __func__)

#define LOG_ERROR(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Error,    (msg), __FILE__, __LINE__, __func__)

#define LOG_CRITICAL(msg) \
    ::logging::detail::PrintWithLocation(LogLevel::Critical, (msg), __FILE__, __LINE__, __func__)
