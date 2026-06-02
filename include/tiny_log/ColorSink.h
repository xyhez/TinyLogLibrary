#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "Level.h"
#include "Sink.h"
#include "SimpleFormatter.h"
#include "detail/NullMutex.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// windows.h 通过宏污染了一堆通用名字，undef 掉避免和我们的成员函数冲突
// （比如 r.GetMessage() 会被替换成 r.GetMessageA()）
#ifdef GetMessage
#undef GetMessage
#endif
#endif

namespace logging {

namespace detail {

#ifdef _WIN32
/**
 * @brief 启用 Windows 控制台的 Virtual Terminal Processing
 *
 * 旧 cmd 默认不识别 ANSI 转义码，会把 "\033[31m" 原样打出来。
 * 调用 SetConsoleMode 启用后才能正确解析颜色。
 */
inline bool EnableVirtualTerminal() {
    auto enable = [](DWORD which) -> bool {
        HANDLE h = GetStdHandle(which);
        if (h == INVALID_HANDLE_VALUE) return false;
        DWORD mode = 0;
        if (!GetConsoleMode(h, &mode)) return false;
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        return SetConsoleMode(h, mode) != 0;
    };
    // stdout 和 stderr 各自的 mode 独立，要分别启用
    bool ok1 = enable(STD_OUTPUT_HANDLE);
    bool ok2 = enable(STD_ERROR_HANDLE);
    return ok1 && ok2;
}
#endif

/**
 * @brief level → ANSI 前景色码
 *
 * 返回 const char* 指向静态字面量，热路径上零分配。
 */
inline const char* ColorOf(Level level) {
    switch (level) {
        case Level::Trace:    return "\033[37m";   // 白
        case Level::Debug:    return "\033[36m";   // 青
        case Level::Info:     return "\033[32m";   // 绿
        case Level::Warn:     return "\033[33m";   // 黄
        case Level::Error:    return "\033[31m";   // 红
        case Level::Critical: return "\033[35m";   // 紫
        default:              return "";
    }
}

/// ANSI 重置码——必须在每条日志末尾输出，否则颜色会污染后续输出
constexpr const char* kColorReset = "\033[0m";

} // namespace detail

/**
 * @brief 控制台彩色输出 Sink
 *
 * 行为对齐 ConsoleSink：
 *   - Error/Critical → std::cerr
 *   - 其它           → std::cout
 *
 * 颜色码逻辑写在 sink 而非 formatter——颜色和"输出到哪"强绑定，
 * Formatter 保持纯净（生成纯文本）。
 *
 * Windows 平台首次构造时会启用 Virtual Terminal Processing，
 * 让旧 cmd 也能识别 ANSI 转义。
 */
template<typename Mutex>
class ColorSink : public BaseSink<Mutex> {
    using BaseSink<Mutex>::formatter_;

public:
    ColorSink() {
#ifdef _WIN32
        // C++11 起 static 局部变量初始化线程安全且只执行一次——天然 call_once
        static bool inited = detail::EnableVirtualTerminal();
        (void)inited;
#endif
        formatter_ = std::make_unique<SimpleFormatter>();
    }

protected:
    void SinkItImpl(const Record& record) override {
        auto& out = (record.GetLevel() >= Level::Error) ? std::cerr : std::cout;
        out << detail::ColorOf(record.GetLevel())
            << formatter_->Format(record)
            << detail::kColorReset
            << '\n';
    }

    void FlushImpl() override {
        std::cout.flush();
        std::cerr.flush();
    }
};

using ColorSinkST = ColorSink<detail::NullMutex>;
using ColorSinkMT = ColorSink<std::mutex>;

} // namespace logging
