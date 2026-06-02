#include <tiny_log/Logging.h>
#include <tiny_log/ColorSink.h>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ============================================================
    // P9.A4：ColorSink 着色测试
    // ============================================================

    auto* color_logger = logging::GetLogger("ColorTest");
    color_logger->ClearSinks();   // 清掉 Registry 自动加的 ConsoleSink
    color_logger->AddSink(std::make_shared<logging::ColorSinkMT>());
    color_logger->SetLevel(logging::Level::Trace);

    LOG_TRACE_TO(color_logger,    "trace - 白色");
    LOG_DEBUG_TO(color_logger,    "debug - 青色");
    LOG_INFO_TO(color_logger,     "info - 绿色");
    LOG_WARN_TO(color_logger,     "warn - 黄色");
    LOG_ERROR_TO(color_logger,    "error - 红色");
    LOG_CRITICAL_TO(color_logger, "critical - 紫色");

    // 验证颜色不会污染下一行普通输出（ANSI reset 生效）
    std::cout << "下一行不应该有颜色\n";

    return 0;
}
