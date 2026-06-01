#pragma once

#include <memory>
#include <sstream>
#include <string>

#include "ConsoleSink.h"
#include "FileSink.h"
#include "Logger.h"
#include "RotatingFileSink.h"
#include "SourceLocation.h"
#include "detail/NullMutex.h"
#include "detail/Registry.h"

// CMake 通过 target_compile_definitions 注入项目根的绝对路径
// 没用 CMake 构建时退化为 "."（当前目录），保证总是能编过
#ifndef TINY_LOG_PROJECT_ROOT
#define TINY_LOG_PROJECT_ROOT "."
#endif

namespace logging {


/**
 * @brief 获取默认 Logger（进程内单例）
 *
 * 必须 inline，否则放在头文件里被多个 TU 包含会触发 ODR 多重定义。
 */
inline Logger* GetDefaultLogger() {
    return detail::Registry::GetInstance()->GetDefaultLogger();
}

/**
 * @brief 按名字获取 Logger，不存在则由 Registry 自动创建
 *
 * 用法：auto* net = logging::GetLogger("Network");
 */
inline Logger* GetLogger(const std::string& name) {
    return detail::Registry::GetInstance()->GetLogger(name);
}

namespace detail {

inline SourceLocation MakeLocation(const char* file, int line, const char* func) {
    SourceLocation sourceLocation{file,line,func};
    return sourceLocation;
}

inline void PrintWithLocation(Level level,
                              const std::string& msg,
                              const char* file,
                              int line,
                              const char* func) {
    GetDefaultLogger()->Print(level, msg, MakeLocation(file, line, func));
}

} // namespace detail
} // namespace logging


// ============================================================
// 用户使用入口：必须是宏，C++17 下函数捕获不到调用点位置
// ============================================================
//
// 用法：
//     LOG_INFO("hello");
//     LOG_ERROR("something broke: " + reason);
// ============================================================

#define LOG_TRACE(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Trace,    (msg), __FILE__, __LINE__, __func__)

#define LOG_DEBUG(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Debug,    (msg), __FILE__, __LINE__, __func__)

#define LOG_INFO(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Info,     (msg), __FILE__, __LINE__, __func__)

#define LOG_WARN(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Warn,     (msg), __FILE__, __LINE__, __func__)

#define LOG_ERROR(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Error,    (msg), __FILE__, __LINE__, __func__)

#define LOG_CRITICAL(msg) \
    ::logging::detail::PrintWithLocation(::logging::Level::Critical, (msg), __FILE__, __LINE__, __func__)




#define LOG_TRACE_TO(logger, msg) \
    (logger)->Print(::logging::Level::Trace, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

#define LOG_DEBUG_TO(logger, msg) \
    (logger)->Print(::logging::Level::Debug, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

#define LOG_INFO_TO(logger, msg) \
    (logger)->Print(::logging::Level::Info, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

#define LOG_WARN_TO(logger, msg) \
    (logger)->Print(::logging::Level::Warn, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

#define LOG_ERROR_TO(logger, msg) \
    (logger)->Print(::logging::Level::Error, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

#define LOG_CRITICAL_TO(logger, msg) \
    (logger)->Print(::logging::Level::Critical, (msg), \
        ::logging::SourceLocation{__FILE__, __LINE__, __func__})

