#pragma once

#include <chrono>
#include <string>
#include <utility>

#include "Level.h"

namespace logging {

/**
 * @brief 一条日志的数据包
 */
class Record {
public:
    Record(Level level,
           std::string message,
           std::chrono::system_clock::time_point time,
           std::string source_location)
        : level_(level)
        , message_(std::move(message))
        , time_(time)
        , source_location_(std::move(source_location)) {}

    Record() = default;
    ~Record() = default;

    Level GetLevel() const { return level_; }
    std::chrono::system_clock::time_point GetTime() const { return time_; }
    const std::string& GetMessage() const { return message_; }
    const std::string& GetSourceLocation() const { return source_location_; }

private:
    Level level_ = Level::Info;
    std::string message_;
    std::chrono::system_clock::time_point time_ = std::chrono::system_clock::now();
    std::string source_location_;
};

} // namespace logging
