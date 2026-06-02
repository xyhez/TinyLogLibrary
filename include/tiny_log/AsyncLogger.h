#pragma once

#include "Logger.h"
#include "detail/Registry.h"
#include "detail/ThreadPool.h"

namespace logging {
class AsyncLogger : public Logger {
public:
    AsyncLogger(std::string name,
        std::shared_ptr<ThreadPool> pool)
            : Logger(std::move(name))
            , pool_(std::move(pool)){}

    /**
     * @brief 异步版Print：把分发sink丢进线程池
     * @param level
     * @param message
     */
    void Print(Level level,
           const std::string& message,
           SourceLocation source_location) override;
private:
    std::shared_ptr<ThreadPool> pool_;
};
}
