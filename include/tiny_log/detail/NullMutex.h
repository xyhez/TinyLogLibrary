#pragma once

/**
 *  @brief 空Mutex类，用于单线程时匹配模板
 */
namespace logging::detail {
class NullMutex {
public:
    void lock() {}
    void unlock(){}
};
}
