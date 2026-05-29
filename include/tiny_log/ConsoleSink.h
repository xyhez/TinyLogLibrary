#pragma once

#include <iostream>
#include <memory>

#include "Level.h"
#include "Sink.h"
#include "SimpleFormatter.h"
#include "detail/NullMutex.h"

namespace logging {

/**
 * @brief 控制台输出：Error/Critical 走 cerr，其余走 cout
 */
template<typename Mutex>
class ConsoleSink : public BaseSink<Mutex> {
    using BaseSink<Mutex>::formatter_;
public:
    ConsoleSink() {
        formatter_ = std::make_unique<SimpleFormatter>();
    }

    void SinkItImpl(const Record& record) override {
        auto& out = (record.GetLevel() >= Level::Error) ? std::cerr : std::cout;
        out << formatter_->Format(record) << '\n';
    }



    void FlushImpl() override {
        std::cout.flush();
        std::cerr.flush();
    }
};

    using ConsoleSinkST = ConsoleSink<NullMutex>;
    using ConsoleSinkMT = ConsoleSink<std::mutex>;

} // namespace logging
