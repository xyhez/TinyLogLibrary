#pragma once

#include <memory>
#include <utility>

#include "Formatter.h"
#include "Record.h"
#include "SimpleFormatter.h"

namespace logging {

/**
 * @brief Sink 抽象基类：负责把 Record 写到具体目的地（控制台 / 文件 / 网络…）
 *
 * 格式化交给 Formatter，这里只负责"写到哪"。
 */
class Sink {
public:
    Sink() : formatter_(std::make_shared<SimpleFormatter>()) {}
    virtual ~Sink() = default;

    virtual void SinkIt(const Record& record) = 0;
    virtual void Flush() = 0;

    void SetFormatter(std::shared_ptr<Formatter> fmt) {
        formatter_ = std::move(fmt);
    }

protected:
    std::shared_ptr<Formatter> formatter_;
};

} // namespace logging
