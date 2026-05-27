#pragma once

namespace logging {

/**
 * @brief 日志等级
 */
enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off,
};

/**
 * @brief 日志等级 → 字符串
 *
 * 返回 const char*（指向静态字面量）而不是 std::string，避免每次格式化都堆分配。
 * switch 比 if/else 链更易被编译器优化成跳转表。
 */
inline const char* ToString(Level level) {
    switch (level) {
        case Level::Trace:    return "Trace";
        case Level::Debug:    return "Debug";
        case Level::Info:     return "Info";
        case Level::Warn:     return "Warn";
        case Level::Error:    return "Error";
        case Level::Critical: return "Critical";
        case Level::Off:      return "Off";
    }
    return "Unknown";
}

} // namespace logging
