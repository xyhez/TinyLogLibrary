#pragma once
#include <iomanip>
#include <iostream>
#include <ostream>
#include <ctime>
#include <iomanip>

#include "Sink.h"
// 控制台输出，继承Sink

class ConsoleSink : public Sink{
public:
    ConsoleSink() = default;
    ~ConsoleSink() = default;

    /**
     * @brief 把日志输出到控制台
     * @param record 包装后的日志信息
     */
    void SinkIt(const LogRecord& record) override {
        // TODO:解析Record然后输出到控制台
        if (record.GetLevel() == LogLevel::Trace
            || record.GetLevel() == LogLevel::Info
            || record.GetLevel() == LogLevel::Debug
            || record.GetLevel() == LogLevel::Warn) {
            // 时间转换
            std::time_t time_point = std::chrono::system_clock::to_time_t(record.GetTime());
            // 转换本地时间
            std::tm local_tm = *std::localtime(&time_point);
            std::cout << "\nLevel:" <<LogLevelToString(record.GetLevel())
                    << "\nMessage:" <<record.GetMessage()
                    << "\nTime:" <<std::put_time(&local_tm,"%Y-%m-%d %H:%M:%S")
                    << "\nSource:" <<record.GetSourceLocation()
                    << std::endl;
        } else {
            // 时间转换
            std::time_t time_point = std::chrono::system_clock::to_time_t(record.GetTime());
            // 转换本地时间
            std::tm local_tm = *std::localtime(&time_point);
            std::cerr << "\nLevel:" <<LogLevelToString(record.GetLevel())
                    << "\nMessage:" <<record.GetMessage()
                    << "\nTime:" <<std::put_time(&local_tm,"%Y-%m-%d %H:%M:%S")
                    << "\nSource:" <<record.GetSourceLocation()
                    << std::endl;
        }
    }

    void Flush() override {
        std::cout.flush();
        std::cerr.flush();
    }
};