#include "tiny_log/detail/Registry.h"

#include "tiny_log/ConsoleSink.h"

namespace logging::detail {
Logger* Registry::GetLogger(const std::string &name)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loggers_.find(name);
    if (it != loggers_.end())
        return it->second.get();
    // 不存在则创建
    std::unique_ptr<Logger> logger = std::make_unique<Logger>(name);
    logger->AddSink(std::make_unique<ConsoleSinkST>());
    Logger* ptr = logger.get();
    loggers_[name] = std::move(logger);
    return ptr;
}

Logger *Registry::GetDefaultLogger() {
    return GetLogger("default");
}

void Registry::FlushAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [name, logger] : loggers_) {
        logger->Flush();
    }
}

void Registry::SetGlobalLevel(Level level) {
    global_level_.store(level, std::memory_order_relaxed);
}

void Registry::SetFlushOnLevel(Level level) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [name, logger] : loggers_) {
        logger->SetFlushOnLevel(level);
    }
}

// 静态成员变量类外初始化
Registry* Registry::registry_ = new Registry();
}

