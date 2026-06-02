/**
 * @file benchmark.cpp
 * @brief 性能基准：各种 sink / 同步 vs 异步 / ST vs MT 的对比
 *
 * 跑 1 万条日志取业务线程耗时，结果输出到控制台 + 写进 benchmark_result.txt。
 * 测试在单线程业务场景下进行，所有 sink 的文件写入到 logfile/bench_*.log
 */

#include <tiny_log/CallbackSink.h>
#include <tiny_log/ColorSink.h>
#include <tiny_log/Logging.h>
#include <windows.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

constexpr int N = 10000;

template<typename Fn>
long long Measure(Fn&& fn) {
    auto t1 = std::chrono::steady_clock::now();
    fn();
    auto t2 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
}

struct Result {
    std::string name;
    long long us;
};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::vector<Result> results;
    const std::string root = TINY_LOG_PROJECT_ROOT;

    // ========================================================
    // 1. ConsoleSinkST 同步
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchConsoleST");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::ConsoleSinkST>());
        lg->SetLevel(logging::Level::Info);
        // 控制台输出会刷屏，重定向 cout 到 /dev/null 等价物
        std::ofstream null_sink(root + "/logfile/bench_null.txt");
        auto* old_buf = std::cout.rdbuf(null_sink.rdbuf());

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"ConsoleSinkST (sync)", us});

        std::cout.rdbuf(old_buf);
    }

    // ========================================================
    // 2. ConsoleSinkMT 同步——对比锁开销
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchConsoleMT");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::ConsoleSinkMT>());
        lg->SetLevel(logging::Level::Info);
        std::ofstream null_sink(root + "/logfile/bench_null.txt");
        auto* old_buf = std::cout.rdbuf(null_sink.rdbuf());

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"ConsoleSinkMT (sync, 单线程内调用)", us});

        std::cout.rdbuf(old_buf);
    }

    // ========================================================
    // 3. FileSinkMT 同步
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchFile");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::FileSinkMT>(
            root + "/logfile/bench_file.log"));
        lg->SetLevel(logging::Level::Info);

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"FileSinkMT (sync)", us});
    }

    // ========================================================
    // 4. RotatingFileSinkMT 同步——含滚动开销
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchRotating");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::RotatingFileSinkMT>(
            root + "/logfile/bench_rotating.log",
            /*max_size=*/100 * 1024,
            /*max_files=*/3));
        lg->SetLevel(logging::Level::Info);

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"RotatingFileSinkMT (sync, max=100KB)", us});
    }

    // ========================================================
    // 5. AsyncLogger + FileSinkMT
    //    注意：这里只测"业务线程入队"耗时，IO 在后台线程做
    //    （Registry 析构时才等队列消费完，不计入此时间）
    // ========================================================
    {
        auto* lg = logging::GetAsyncLogger("BenchAsyncFile");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::FileSinkMT>(
            root + "/logfile/bench_async_file.log"));
        lg->SetLevel(logging::Level::Info);

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"AsyncLogger + FileSinkMT (业务线程入队)", us});
    }

    // ========================================================
    // 6. CallbackSink（无 IO，纯函数调用）
    // ========================================================
    {
        long long counter = 0;
        auto* lg = logging::GetLogger("BenchCallback");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::CallbackSink>(
            [&counter](const logging::Record&) { ++counter; }));
        lg->SetLevel(logging::Level::Info);

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "bench msg");
        });
        results.push_back({"CallbackSink (no IO, 仅函数调用)", us});
    }

    // ========================================================
    // 7. LOG_INFO_FMT vs LOG_INFO（拼接成本）
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchFmt");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::CallbackSink>(
            [](const logging::Record&) {}));   // 空回调，专测 format 开销
        lg->SetLevel(logging::Level::Info);

        // a. 字符串拼接
        auto us_concat = Measure([&] {
            for (int i = 0; i < N; ++i) {
                LOG_INFO_TO(lg, "user=" + std::string("wang") +
                                " age=" + std::to_string(22) +
                                " score=" + std::to_string(95.6));
            }
        });
        results.push_back({"LOG_INFO_TO (string +)", us_concat});

        // b. std::format 风格
        auto us_fmt = Measure([&] {
            for (int i = 0; i < N; ++i) {
                LOG_INFO_FMT_TO(lg, "user={} age={} score={}", "wang", 22, 95.6);
            }
        });
        results.push_back({"LOG_INFO_FMT_TO (std::format)", us_fmt});
    }

    // ========================================================
    // 8. 过滤掉的日志（最快路径）
    // ========================================================
    {
        auto* lg = logging::GetLogger("BenchFiltered");
        lg->ClearSinks();
        lg->AddSink(std::make_shared<logging::CallbackSink>(
            [](const logging::Record&) {}));
        lg->SetLevel(logging::Level::Error);   // Trace/Debug/Info 全过滤

        auto us = Measure([&] {
            for (int i = 0; i < N; ++i) LOG_INFO_TO(lg, "filtered out");
        });
        results.push_back({"被过滤的日志 (early return)", us});
    }

    // ========================================================
    // 输出结果
    // ========================================================
    std::cout << "\n=== Benchmark Results (" << N << " 条 / 业务线程总耗时) ===\n";
    std::cout << std::string(60, '-') << '\n';
    std::cout << std::left;

    auto print_row = [](std::ostream& os, const Result& r) {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%-50s %8lld us  (%6.2f us/op)",
                      r.name.c_str(), r.us, double(r.us) / N);
        os << buf << '\n';
    };

    for (auto& r : results) print_row(std::cout, r);

    // 写到文件方便引用
    std::ofstream out(root + "/docs/benchmark_result.txt");
    out << "Benchmark Results (" << N << " 条 / 业务线程总耗时)\n";
    out << std::string(60, '-') << '\n';
    for (auto& r : results) print_row(out, r);

    std::cout << "\n结果已写入 docs/benchmark_result.txt\n";

    return 0;
}
