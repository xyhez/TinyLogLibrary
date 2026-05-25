#pragma once
#include "Sink.h"
// 控制台输出，继承Sink

class FileSink : public Sink{
public:
    FileSink() = default;
    ~FileSink() = default;

    /**
     * @brief 把日志输出到控制台
     * @param record 包装后的日志信息
     */
    void SinkIt(const LogRecord& record) override {
        // TODO:解析Record输出到文件
    }

    void Flush() override {

    }
};