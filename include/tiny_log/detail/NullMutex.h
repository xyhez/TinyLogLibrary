#pragma once

/**
 *  @brief 空Mutex类，用于单线程时匹配模板
 */
namespace logging {
class NullMutex {
public:
    void lock() {
        std::cout << "NullMutex lock" << std::endl;
    }
    void unlock(){}
};
}
