#pragma once
#include <string>

enum class LogLevel {
    Trace,  // 跟踪信息
    Debug,  // 调试信息
    Info,   // 一般信息
    Warn,   // 警告
    Error,  //错误
    Critical,// 严重错误
    Off     // 关闭日志
};

inline std::string LogLevelToString(LogLevel level) {
    if (level == LogLevel::Trace) {
        return "Trace";
    }else if (level == LogLevel::Debug) {
        return "Debug";
    } else if (level == LogLevel::Info) {
        return "Info";
    } else if (level == LogLevel::Warn) {
        return "Warn";
    } else if (level == LogLevel::Error) {
        return "Error";
    } else if (level == LogLevel::Critical) {
        return "Critical";
    }else if (level == LogLevel::Off) {
        return "Off";
    }
    return " ";
}
