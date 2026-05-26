#pragma once

#include <iostream>
#include "LogRecord.h"
#include "LogLevel.h"
#include "Tools.h"

/**
 *@brief 格式化
 */
class IFormatter {
public:
    virtual ~IFormatter() = default;

    virtual std::string Format(const LogRecord &record) = 0;
};

class SimpleFormatter : public IFormatter {
public:
    std::string Format(const LogRecord &record) override{
        std::ostringstream oss;
        oss << "[" << LogLevelToString(record.GetLevel()) << "] "
            << record.GetMessage() << record.GetSourceLocation();
        return oss.str();
    }
};

class FullFormatter : public IFormatter {
public:
    std::string Format(const LogRecord &record) override {
        std::ostringstream oss;
        oss << "[" << TimeToString(record.GetTime()) << "] "
            << "[" << record.GetSourceLocation() << "] "
            << "[" << LogLevelToString(record.GetLevel()) << "] "
            << record.GetMessage();
        return oss.str();
    }
};