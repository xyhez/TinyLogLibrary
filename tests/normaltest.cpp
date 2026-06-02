#include <tiny_log/Logging.h>
#include <tiny_log/CallbackSink.h>
#include <tiny_log/ColorSink.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ============================================================
    // P9.A4：ColorSink 着色测试
    // ============================================================
    {
        auto* color_logger = logging::GetLogger("ColorTest");
        color_logger->ClearSinks();
        color_logger->AddSink(std::make_shared<logging::ColorSinkMT>());
        color_logger->SetLevel(logging::Level::Trace);

        LOG_TRACE_TO(color_logger,    "trace - 白色");
        LOG_DEBUG_TO(color_logger,    "debug - 青色");
        LOG_INFO_TO(color_logger,     "info - 绿色");
        LOG_WARN_TO(color_logger,     "warn - 黄色");
        LOG_ERROR_TO(color_logger,    "error - 红色");
        LOG_CRITICAL_TO(color_logger, "critical - 紫色");
    }

    // ============================================================
    // P9.A3：CallbackSink 测试
    // ============================================================

    // 1. 基本回调：捕获所有日志到 vector
    {
        std::vector<std::string> captured;
        auto cb = std::make_shared<logging::CallbackSink>(
            [&captured](const logging::Record& r) {
                captured.push_back(r.GetMessage());
            });

        auto* logger = logging::GetLogger("CbBasic");
        logger->ClearSinks();
        logger->AddSink(cb);

        LOG_INFO_TO(logger, "msg1");
        LOG_WARN_TO(logger, "msg2");
        LOG_ERROR_TO(logger, "msg3");

        std::cout << "\n[CbBasic] captured " << captured.size() << " 条 (期望 3):\n";
        for (auto& m : captured) std::cout << "  - " << m << '\n';
    }

    // 2. 按 level 筛选：只关心 Error 及以上
    {
        int error_count = 0;
        auto cb = std::make_shared<logging::CallbackSink>(
            [&error_count](const logging::Record& r) {
                if (r.GetLevel() >= logging::Level::Error) ++error_count;
            });

        auto* logger = logging::GetLogger("CbFilter");
        logger->ClearSinks();
        logger->AddSink(cb);

        LOG_INFO_TO(logger, "info");
        LOG_WARN_TO(logger, "warn");
        LOG_ERROR_TO(logger, "error");
        LOG_CRITICAL_TO(logger, "critical");

        std::cout << "\n[CbFilter] error_count = " << error_count << " (期望 2)\n";
    }

    // 3. 多 sink 共存：CallbackSink + ColorSink 一起工作
    {
        int callback_hits = 0;
        auto cb = std::make_shared<logging::CallbackSink>(
            [&callback_hits](const logging::Record&) { ++callback_hits; });

        auto* logger = logging::GetLogger("CbMixed");
        logger->ClearSinks();
        logger->AddSink(std::make_shared<logging::ColorSinkMT>());
        logger->AddSink(cb);

        std::cout << "\n[CbMixed] 同一条日志同时走 ColorSink + CallbackSink:\n";
        LOG_INFO_TO(logger, "this goes to both color and callback");
        LOG_WARN_TO(logger, "this too");

        std::cout << "[CbMixed] callback_hits = " << callback_hits << " (期望 2)\n";
    }

    return 0;
}
