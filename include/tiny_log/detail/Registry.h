#pragma once
#include <memory>
#include <unordered_map>

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
    // 禁用拷贝构造和赋值运算符
    Registry(const Registry& obj) = delete;
    Registry& operator=(const Registry& obj) = delete;

private:
    // 私有构造函数
    Registry() = default;

    ///< 单例
    static Registry* registry_;

    std::shared_ptr<Registry> default_logger_ = nullptr; // 默认
    std::unordered_map<std::string, std::shared_ptr<Registry>> Loggers_;

};
// 静态成员变量类外初始化
Registry* Registry::registry_ = new Registry();
}
