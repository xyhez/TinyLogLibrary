#pragma once

#include <iostream>
#include <memory>

#include "Level.h"
#include "Sink.h"
#include "SimpleFormatter.h"

namespace logging {

/**
 * @brief 控制台输出：Error/Critical 走 cerr，其余走 cout
 */
class ConsoleSink : public BaseSink {
public:
    ConsoleSink() {
        formatter_ = std::make_unique<SimpleFormatter>();
    }

    void SinkIt(const Record& record) override {
        auto& out = (record.GetLevel() >= Level::Error) ? std::cerr : std::cout;
        out << formatter_->Format(record) << '\n';
    }

    void Flush() override {
        std::cout.flush();
        std::cerr.flush();
    }
};

} // namespace logging
