#pragma once
#include <memory>
#include <unordered_map>
#include "tiny_log/Logger.h"
#include "tiny_log/ConsoleSink.h"
#include "tiny_log/RotatingFileSink.h"

/**
 * @brief 通过Registry管理项目中的所有Logger，适配不同模块对日志输出的不同需求
 *
 * 单例模式，内部管理静态对象，不允许外界创建，只允许获取
 */
namespace logging::detail {
class Registry {
public:

    static Registry* GetInstance() {
        return registry_;
    }

    /**
     * @brief 根据name返回logger
     * @param name
     * @return
     */
    Logger *GetLogger(const std::string &name);

    /**
     * @brief 返回默认Logger
     * @return
     */
    Logger* GetDefaultLogger();

    /**
     * @brief 刷新所有L
     */
    void FlushAll();

    // 禁用拷贝构造和赋值运算符
    Registry(const Registry& obj) = delete;
    Registry& operator=(const Registry& obj) = delete;
    ~Registry() = default;

private:
    // 私有构造函数
    Registry() {
        // 单例构造时 "default" Logger创建好
        std::unique_ptr<Logger> def = std::make_unique<Logger>("default");
        def->AddSink(std::make_shared<ConsoleSinkMT>());
        def->AddSink(std::make_shared<RotatingFileSinkMT>(
            std::string(TINY_LOG_PROJECT_ROOT) + "/logfile/log1.txt", 50000, 3));
        loggers_["default"] = std::move(def);
    }

    static Registry* registry_;     ///< 单例

    std::unordered_map<std::string, std::unique_ptr<Logger>> loggers_;
    std::mutex mutex_;

};
}
