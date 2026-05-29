#pragma once

#include <memory>
#include <mutex>
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
    Sink() = default;
    virtual ~Sink() = default;

    virtual void SinkIt(const Record& record) = 0;
    virtual void Flush() = 0;

};

template<typename Mutex>
class BaseSink : public Sink {
public:
    BaseSink() = default;
    ~BaseSink() override = default;

    void SetFormatter(std::unique_ptr<Formatter> fmt) {
        std::lock_guard<Mutex> lock(mutex_);
        formatter_ = std::move(fmt);
    }

    void SinkIt(const Record &record) final {
        std::lock_guard<Mutex> lock(mutex_);
        SinkItImpl(record);
    }

    void Flush() final{
        std::lock_guard<Mutex> lock(mutex_);
        FlushImpl();
    }

protected:
    virtual void SinkItImpl(const Record& record) = 0;
    virtual void FlushImpl() = 0;


    std::unique_ptr<Formatter> formatter_;
    Mutex mutex_;
};

} // namespace logging
