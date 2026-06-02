#pragma once

#include <functional>
#include <utility>

#include "Record.h"
#include "Sink.h"

namespace logging {

/**
 * @brief 把 Record 原样回调给用户函数的 Sink
 *
 * 不持有 Formatter——结构化的 Record 直接交给回调，用户自己决定怎么处理：
 *   - 上报监控系统（Sentry / Loki / 自家告警）
 *   - 单元测试断言日志
 *   - 转发到 UI（Qt / 网页 console）
 *   - 限流、去重、采样
 *
 * 用法：
 * @code
 *     auto sink = std::make_shared<CallbackSink>([](const Record& r) {
 *         if (r.GetLevel() >= Level::Error) {
 *             my_monitor.Report(r.GetMessage());
 *         }
 *     });
 *     logger->AddSink(sink);
 * @endcode
 *
 * 设计要点：
 *   1. 直接继承 Sink 接口而非 BaseSink<Mutex>——不需要 Formatter，回调拿
 *      原始 Record 自由发挥
 *   2. 不加锁——锁的粒度由用户根据回调内容决定（比如发网络可能是异步无锁，
 *      改 vector 可能要 mutex）
 *   3. 构造时一次性绑定回调，运行时不可变——避免并发换函数指针的麻烦
 */
class CallbackSink : public Sink {
public:
    using Callback = std::function<void(const Record&)>;

    explicit CallbackSink(Callback cb) : cb_(std::move(cb)) {}

    void SinkIt(const Record& record) override {
        // 防御默认构造的 std::function 被传进来调用抛 bad_function_call
        if (cb_) cb_(record);
    }

    void Flush() override {
        // 回调用户的事，库这边没有 buffer 可刷
    }

private:
    Callback cb_;
};

} // namespace logging
