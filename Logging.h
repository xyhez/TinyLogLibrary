#pragma once

#include "Logger.h"
#include <string>
#include <memory>

#include "ConsoleSink.h"

namespace logging {

    // 获取默认 Logger（全局单例）
    Logger* GetDefaultLogger() {
        static Logger default_logger;
        static bool initialized = false;
        if (!initialized) {
            default_logger.AddSink(std::make_shared<ConsoleSink>());
            initialized = true;
        }
        return &default_logger;
    }


    /**
     * @brief 跟踪信息入口
     * @param msg
     */
    inline void Trace(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Trace, msg);
    }

    /**
     * @brief 调试信息入口
     * @param msg
     */
    inline void Debug(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Debug, msg);
    }

    /**
     * @brief 一般信息入口
     * @param msg
     */
    inline void Info(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Info, msg);
    }

    /**
     * @brief 警告信息入口
     * @param msg
     */
    inline void Warn(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Warn, msg);
    }

    /**
     * @brief 错误信息入口
     * @param msg
     */
    inline void Error(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Error, msg);
    }

    /**
     * @brief 严重信息入口
     * @param msg
     */
    inline void Critical(const std::string& msg) {
        GetDefaultLogger()->Print(LogLevel::Critical, msg);
    }

} // namespace logging