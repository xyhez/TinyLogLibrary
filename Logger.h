#pragma once
#include <memory>
#include <vector>

#include "LogLevel.h"
#include "Sink.h"

/**
 * @brief 日志器
 */
class Logger {
public:
    Logger() = default;

    /**
     * @brief 核心打印方法（不带源码位置）
     */
    void Print(LogLevel level, const std::string& message);

    /**
     * @brief 核心打印方法（带源码位置，宏入口走这个）
     */
    void Print(LogLevel level,
               const std::string& message,
               const std::string& source_location);

    /**
     * @brief 添加Sink
     * @param sink
     */
    void AddSink(std::shared_ptr<Sink> sink);

    /**
     * @brief 清理所有Sink
     */
    void ClearSinks();

    /**
     * @brief 刷新所有Sink
     */
    void Flush();

private:
    ///< 分发器列表
    std::vector<std::shared_ptr<Sink>> sinks_;
};

