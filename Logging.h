#pragma once

#include "Logger.h"
#include <string>

namespace logging {

    // 获取默认 Logger（全局单例）
    Logger* GetDefaultLogger();

    // ---------- 函数式入口 ----------
    // 第一版不引入 fmt，直接用 std::string

    inline void Debug(const std::string& msg) {
        GetDefaultLogger()->Log(LogLevel::Debug, msg);
    }
    inline void Info(const std::string& msg) {
        GetDefaultLogger()->Log(LogLevel::Info, msg);
    }
    inline void Warn(const std::string& msg) {
        GetDefaultLogger()->Log(LogLevel::Warn, msg);
    }
    inline void Error(const std::string& msg) {
        GetDefaultLogger()->Log(LogLevel::Error, msg);
    }
    inline void Critical(const std::string& msg) {
        GetDefaultLogger()->Log(LogLevel::Critical, msg);
    }

} // namespace logging