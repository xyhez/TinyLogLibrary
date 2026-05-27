#pragma once

#include <sstream>
#include <string>

#include "Formatter.h"
#include "Level.h"
#include "detail/TimeUtils.h"

namespace logging {

/**
 * @brief 完整格式：[time] [location] [Level] message
 *
 * 适合文件输出。
 */
class FullFormatter : public Formatter {
public:
    std::string Format(const Record& record) override {
        std::ostringstream oss;
        oss << "[" << detail::TimeToString(record.GetTime()) << "] "
            << "[" << record.GetSourceLocation() << "] "
            << "[" << ToString(record.GetLevel()) << "] "
            << record.GetMessage();
        return oss.str();
    }
};

} // namespace logging
