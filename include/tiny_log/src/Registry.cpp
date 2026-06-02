#include "tiny_log/detail/Registry.h"

#include "tiny_log/AsyncLogger.h"
#include "tiny_log/ConsoleSink.h"

namespace logging::detail {

Registry::Registry() {
    // 1. 全局共享的 ThreadPool：默认配置（core=5, max=10, 队列=30），
    //    可按需调小。destroy_timeout_=0 → ShutdownNow���直接丢任务），
    //    所以析构时要走主动 Shutdown() 路径等待消费完。
    thread_pool_ = std::make_shared<ThreadPool>(ThreadPoolConfig{});

    // 2. 默认 Logger：Console + RotatingFile
    auto def = std::make_unique<Logger>("default");
    def->AddSink(std::make_shared<ConsoleSinkMT>());
    def->AddSink(std::make_shared<RotatingFileSinkMT>(
        std::string(TINY_LOG_PROJECT_ROOT) + "/logfile/log.txt", 50000, 3));
    loggers_["default"] = std::move(def);
}

Registry::~Registry() {
    // 1. 关闭线程池前先 Shutdown，等待队列里的日志 task 全部消费完，
    //    再让 loggers_ map 析构（unique_ptr 自动释放 Logger）。
    if (thread_pool_) {
        thread_pool_->Shutdown();
    }
    // loggers_ 自动析构
}

Logger* Registry::GetLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loggers_.find(name);
    if (it != loggers_.end())
        return it->second.get();

    auto logger = std::make_unique<Logger>(name);
    logger->AddSink(std::make_shared<ConsoleSinkST>());
    Logger* ptr = logger.get();
    loggers_[name] = std::move(logger);
    return ptr;
}

Logger* Registry::GetAsyncLogger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = loggers_.find(name);
    if (it != loggers_.end())
        return it->second.get();

    // 异步 Logger 共享全局 ThreadPool
    auto logger = std::make_unique<AsyncLogger>(name, thread_pool_);
    logger->AddSink(std::make_shared<ConsoleSinkMT>());
    Logger* ptr = logger.get();
    loggers_[name] = std::move(logger);
    return ptr;
}

Logger* Registry::GetDefaultLogger() {
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
