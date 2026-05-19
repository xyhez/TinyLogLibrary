#pragma once
#include "LogRecord.h"
// 抽象接口，是所有XxxSink的父类，是所有Sink实现的抽象，Logger只关心

/**
 * @brief Sink的主要职责为提供统一接口，待子类去实现，在Logger中可以使用Sink*管理所有
 */
class Sink {
    public:
    Sink() = default;
    ~Sink() = default;

    virtual void Write(const Record& record) = 0;

};