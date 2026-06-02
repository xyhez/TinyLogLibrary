#include <tiny_log/Logging.h>
#include <windows.h>

#include <chrono>
#include <thread>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // ============================================================
    // P8：异步 Logger 基本验证
    // ============================================================

    auto* async_net = logging::GetAsyncLogger("AsyncNet");
    async_net->SetLevel(logging::Level::Trace);

    LOG_INFO_TO(async_net, "async log #1 - 业务线程立刻返回");
    LOG_INFO_TO(async_net, "async log #2");
    LOG_INFO_TO(async_net, "async log #3");

    // ============================================================
    // P8：业务线程加速验证（同步 vs 异步）
    // ============================================================

    constexpr int N = 1000;

    // 同步：业务线程亲自做 IO
    auto* sync_logger = logging::GetLogger("SyncBench");
    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        LOG_INFO_TO(sync_logger, "sync benchmark");
    }
    auto t2 = std::chrono::steady_clock::now();
    auto sync_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    // 异步：业务线程只入队
    auto* async_bench = logging::GetAsyncLogger("AsyncBench");
    auto t3 = std::chrono::steady_clock::now();
    for (int i = 0; i < N; ++i) {
        LOG_INFO_TO(async_bench, "async benchmark");
    }
    auto t4 = std::chrono::steady_clock::now();
    auto async_us = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

    LOG_INFO("sync 1000 条耗时 (us): " + std::to_string(sync_us));
    LOG_INFO("async 1000 条入队耗时 (us): " + std::to_string(async_us));
    LOG_INFO("加速比 (sync/async): " + std::to_string((double)sync_us / async_us));

    // ============================================================
    // P8：落盘完整性验证
    // ============================================================
    // main 退出前 Registry 析构会 Shutdown ThreadPool，等队列消费完。
    // 不需要手动 sleep。

    return 0;
}
