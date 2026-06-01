#include <tiny_log/Logging.h>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ============================================================
    // P7.1：默认 Logger / 命名 Logger
    // ============================================================

    LOG_INFO("default logger - info");
    LOG_WARN("default logger - warn");

    auto* net = logging::GetLogger("Network");
    net->SetLevel(logging::Level::Debug);

    LOG_TRACE_TO(net, "不该出现 (Trace 低于 Debug)");
    LOG_DEBUG_TO(net, "network debug - 应该出现");
    LOG_INFO_TO(net, "network info - 应该出现");

    // ============================================================
    // P7.2：全局 level（双层过滤）
    // ============================================================

    // 1. 全局静音：所有日志被全局上限拦掉
    logging::SetGlobalLevel(logging::Level::Off);
    LOG_INFO("不该出现 (全局 Off)");
    LOG_DEBUG_TO(net, "不该出现 (全局 Off)");
    LOG_ERROR_TO(net, "不该出现 (全局 Off)");

    // 2. 恢复全局：各 Logger 自己的 level 配置仍然生效
    logging::SetGlobalLevel(logging::Level::Trace);
    LOG_DEBUG_TO(net, "应该出现 (全局放行 + Logger=Debug)");

    // 3. 全局上限可以缩小：即使 Logger 想打 Debug，全局 Warn 也拦
    logging::SetGlobalLevel(logging::Level::Warn);
    LOG_DEBUG_TO(net, "不该出现 (全局 Warn 拦了 Debug)");
    LOG_WARN_TO(net, "应该出现 (全局 Warn 放行 Warn)");

    // 恢复全局放行
    logging::SetGlobalLevel(logging::Level::Trace);

    // ============================================================
    // P7.2：flush_on_level
    // ============================================================

    // 默认 Critical 才立即 flush；改成 Error 之后 Error/Critical 都立即落盘
    logging::SetFlushOnLevel(logging::Level::Error);
    LOG_INFO_TO(net, "不立即 flush (低于 Error)");
    LOG_ERROR_TO(net, "立即 flush (达到阈值)");

    // ============================================================
    // P7.2：FlushAll
    // ============================================================

    logging::FlushAll();   // 程序退出前手动刷盘所有 Logger

    return 0;
}
