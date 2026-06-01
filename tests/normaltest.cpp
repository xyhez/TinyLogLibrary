#include <tiny_log/Logging.h>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ---------- 默认 Logger ----------
    LOG_INFO("default logger - info");
    LOG_WARN("default logger - warn");

    // ---------- 命名 Logger：Network ----------
    // 不存在则自动创建，继承默认配置
    auto* net = logging::GetLogger("Network");
    net->SetLevel(logging::Level::Debug);   // 阈值降到 Debug，看更多细节

    LOG_TRACE_TO(net, "不该出现 (Trace 低于 Debug)");
    LOG_DEBUG_TO(net, "network debug - 应该出现");
    LOG_INFO_TO(net, "network info - 应该出现");
    LOG_ERROR_TO(net, "network error - 应该出现");

    // ---------- 命名 Logger：Render ----------
    // 阈值抬到 Warn，过滤 Info 以下
    auto* render = logging::GetLogger("Render");
    render->SetLevel(logging::Level::Warn);

    LOG_INFO_TO(render, "不该出现 (Info 低于 Warn)");
    LOG_WARN_TO(render, "render warn - 应该出现");
    LOG_CRITICAL_TO(render, "render critical - 应该出现");

    // ---------- 复用同名 Logger ----------
    // 第二次 GetLogger("Network") 应该返回同一个实例
    auto* net2 = logging::GetLogger("Network");
    if (net == net2) {
        LOG_INFO_TO(net2, "Registry 复用同名 Logger - 应该出现");
    } else {
        LOG_ERROR_TO(net2, "Registry 没复用 - bug！");
    }

    return 0;
}
