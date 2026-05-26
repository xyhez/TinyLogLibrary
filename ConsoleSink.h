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
    ConsoleSink() {
        formatter_ = std::make_shared<SimpleFormatter>();
    }
    ~ConsoleSink() override = default;

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

            std::cout << formatter_->Format(record) << std::endl;
        } else {
            std::cerr << formatter_->Format(record) << std::endl;
        }
    }

    void Flush() override {
        std::cout.flush();
        std::cerr.flush();
    }
};