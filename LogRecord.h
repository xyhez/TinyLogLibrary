#pragma once
#include <string>
#include <chrono>
#include "LogLevel.h"

/**
 * @brief 日志内容包装器
 */
class Record{
public:
    Record(LogLevel level,
           std::string message,
           std::chrono::system_clock::time_point time,
           std::string source_location)
           : level_(level)
           , message_(message)
           , time_(time)
           , source_location_(source_location){

    }
    Record() = default;
    ~Record() = default;
private:
    ///< level - 默认为Info
    LogLevel level_ = LogLevel::Info;
    ///< message - 默认为空
    std::string message_ = "";
    ///< time - 默认为日志创建的时间
    std::chrono::system_clock::time_point time_ = std::chrono::system_clock::now();
    ///< source_location- 源码位置
    std::string source_location_ = "";
};
