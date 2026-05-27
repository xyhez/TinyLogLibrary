#pragma once

#include <string>

#include "Record.h"

namespace logging {

/**
 * @brief 格式化器接口：把 Record 转成最终要写出去的字符串
 */
class Formatter {
public:
    virtual ~Formatter() = default;
    virtual std::string Format(const Record& record) = 0;
};

} // namespace logging
