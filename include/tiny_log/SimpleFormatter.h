#pragma once

#include <sstream>
#include <string>

#include "Formatter.h"
#include "Level.h"

namespace logging {

/**
 * @brief 简单格式：[Level] message [location]
 *
 * 适合控制台输出。
 */
class SimpleFormatter : public Formatter {
public:
    std::string Format(const Record& record) override {
        std::ostringstream oss;
        oss << "[" << ToString(record.GetLevel()) << "] "
            << record.GetMessage()
            << " [" << record.GetSourceLocation().file << ":" << record.GetSourceLocation().line << "]";
        return oss.str();
    }
};

} // namespace logging
