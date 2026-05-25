#pragma once
#include "LogRecord.h"
#include "Formatter.h"
// 抽象接口，是所有XxxSink的父类，是所有Sink实现的抽象，Logger只关心

/**
 * @brief Sink的主要职责为提供统一接口，待子类去实现，在Logger中可以使用Sink*管理所有
 */
class Sink {
    public:
    Sink() = default;
    virtual ~Sink() = default;

    virtual void SinkIt(const LogRecord& record) = 0;
    virtual void Flush() = 0;

    void SetFormatter(std::shared_ptr<Fomatter> fmt) {
        formatter_ = fmt;
    }
private:
    std::shared_ptr<Fomatter> formatter_;


};