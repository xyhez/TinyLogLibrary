#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
#include "tiny_log/Logger.h"
#include "tiny_log/ConsoleSink.h"
#include "tiny_log/RotatingFileSink.h"
#include "tiny_log/detail/ThreadPool.h"

/**
 * @brief 通过Registry管理项目中的所有Logger，适配不同模块对日志输出的不同需求
 *
 * 单例模式，内部管理静态对象，不允许外界创建，只允许获取
 */
namespace logging {

class AsyncLogger;   // 前向声明，避免 Registry.h ↔ AsyncLogger.h 循环

namespace detail {

class Registry {
public:

    static Registry* GetInstance() {
        return registry_;
    }

    /**
     * @brief 根据name返回logger
     */
    Logger *GetLogger(const std::string &name);

    /**
     * @brief 根据 name 返回 AsyncLogger，不存在则创建（共享全局 ThreadPool）
     */
    Logger* GetAsyncLogger(const std::string& name);

    /**
     * @brief 获取全局共享的 ThreadPool（异步日志专用）
     */
    std::shared_ptr<ThreadPool> GetThreadPool() const { return thread_pool_; }

    /**
     * @brief 返回默认Logger
     */
    Logger* GetDefaultLogger();

    /**
     * @brief 刷新所有Logger
     */
    void FlushAll();

    /**
     * @brief 一键设置全局 level（双层过滤的全局上限）
     */
    void SetGlobalLevel(Level level);

    /**
     * @brief 获取全局 level
     */
    [[nodiscard]] Level GetGlobalLevel() const {
        return global_level_.load(std::memory_order_relaxed);
    }

    /**
    * @brief 一键设置所有 Logger 的 flush_on_level
     */
    void SetFlushOnLevel(Level level);

    Registry(const Registry& obj) = delete;
    Registry& operator=(const Registry& obj) = delete;
    ~Registry();

private:
    Registry();

    static Registry* registry_;     ///< 单例
    std::atomic<Level> global_level_ {Level::Trace};

    std::shared_ptr<ThreadPool> thread_pool_;   ///< 全局共享的线程池（异步日志用）

    std::unordered_map<std::string, std::unique_ptr<Logger>> loggers_;
    std::mutex mutex_;
};

}   // namespace detail
}   // namespace logging
