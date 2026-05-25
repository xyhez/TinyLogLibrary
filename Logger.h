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
     * @brief 核心打印方法
     * @param level
     * @param message
     */
    void Print(LogLevel level, const std::string& message);

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
    std::vector<std::shared_ptr<Sink>> sinks_;
};

