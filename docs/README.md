# TinyLogLibrary

一个 C++20 的轻量级日志库，专为学习现代 C++ 设计模式和并发编程而生。

支持等级过滤、源码位置捕获、多 Sink 分发、滚动文件、ANSI 彩色输出、用户回调、std::format 风格格式化、异步日志，以及单线程/多线程双版本零开销切换。

## 特性

- **6 个日志等级** Trace / Debug / Info / Warn / Error / Critical（+ Off 关闭）
- **双层过滤** Logger 自身 level + Registry 全局 level，`SetGlobalLevel(Off)` 一键静音保留各 Logger 配置
- **源码位置捕获** 宏在调用点展开 `__FILE__`/`__LINE__`/`__func__`，结构化保存到 `SourceLocation`
- **多种 Sink**
    - `ConsoleSink` 控制台输出（Error/Critical 走 stderr）
    - `FileSink` 文件输出
    - `RotatingFileSink` 按大小滚动
    - `ColorSink` ANSI 彩色控制台（Windows 自动启用 Virtual Terminal）
    - `CallbackSink` 用户自定义回调（接监控系统 / 单元测试 / UI 转发）
- **零开销 ST/MT 切换** `BaseSink<Mutex>` 模板，单线程版用 `NullMutex` 编译期消除锁
- **异步日志** `AsyncLogger` 把分发任务丢给全局共享的 ThreadPool，业务线程入队即返回
- **std::format 格式化** `LOG_INFO_FMT("user={} age={}", name, age)`，C++20 编译期类型校验
- **flush_on_level** 关键日志立即落盘，避免 crash 时丢失
- **多 Logger** Registry 单例管理命名 Logger，模块独立配置 level/sink

## Quick Start

```cpp
#include <tiny_log/Logging.h>

int main() {
    // 默认 Logger（Console + RotatingFile）
    LOG_INFO("hello world");
    LOG_ERROR("something failed: {}", "timeout");

    // 模块专属 Logger
    auto* net = logging::GetLogger("Network");
    net->SetLevel(logging::Level::Debug);
    LOG_DEBUG_TO(net, "connecting to {}", "127.0.0.1");

    // 异步 Logger（业务线程不阻塞 IO）
    auto* async_lg = logging::GetAsyncLogger("AsyncBiz");
    LOG_INFO_TO(async_lg, "fire and forget");

    // 全局静音 / 恢复
    logging::SetGlobalLevel(logging::Level::Off);
    LOG_INFO("not printed");
    logging::SetGlobalLevel(logging::Level::Trace);
}
```

输出：

```
[Info] hello world [main.cpp:5 main]
[Error] something failed: timeout [main.cpp:6 main]
[Debug] connecting to 127.0.0.1 [main.cpp:11 main]
[Info] fire and forget [main.cpp:15 main]
```

## 构建

```bash
cmake -B build
cmake --build build

./build/normaltest      # 功能演示
./build/benchmark       # 性能基准
```

要求：

- C++20（用了 `std::format` + `__VA_OPT__`）
- 编译器：GCC 13+ / Clang 15+ / MSVC 19.30+
- CMake 3.16+

## 架构

详见 [`docs/architecture.md`](docs/architecture.md)，包含主调用链路图、类关系图、异步数据流图。

核心理念：

> Logger 管判断和分发 / Record 装数据 / Sink 管"写到哪" / Formatter 管"长什么样"

格式化决策推迟到最后一公里（Sink 调 Formatter），中间层全部传结构化数据——这让 `CallbackSink` 能拿到原始 Record 自由处理，让 `ColorSink` 能在格式化字符串外面包颜色码。

## 设计要点

### 锁的零开销抽象

```cpp
template<typename Mutex>
class BaseSink : public Sink {
    void SinkIt(const Record& r) final {
        std::lock_guard<Mutex> lock(mutex_);   // ← 模板参数静态绑定
        SinkItImpl(r);
    }
    Mutex mutex_;
};

using FileSinkST = FileSink<NullMutex>;        // 单线程：lock_guard 全部内联消除
using FileSinkMT = FileSink<std::mutex>;       // 多线程：真正加锁
```

`NullMutex::lock()/unlock()` 是空函数。`BaseSink<NullMutex>` 实例化后，编译器把 `lock_guard` 整个抹掉——单线程版**没有任何运行时锁开销**，且 `mutex_` 字段通过 EBO 优化也不占空间。

### 异步日志的生产者-消费者

```
业务线程                            后台线程
    │                                  │
    │ LOG_INFO_TO(async, "msg")        │
    v                                  │
AsyncLogger::Print                     │
    │                                  │
    │  双层过滤                         │
    │  构造 Record + 拷贝 sinks 快照    │
    │  ThreadPool::SubmitTask(lambda)  │
    │                                  v
    └──→ 立刻返回                Pop task → SinkIt → 写文件
```

关键设计：

- Record 和 sinks 都按值捕获进 lambda → AsyncLogger 析构后 task 仍能安全执行
- ThreadPool 由 Registry 单例持有，所有 AsyncLogger 共享
- Registry 析构时 `Shutdown()` 等队列消费完才退出，避免日志丢失

### 双层过滤 + atomic 全局 level

```cpp
class Registry {
    std::atomic<Level> global_level_ {Level::Trace};
};

void Logger::Print(...) {
    if (!ShouldLog(level)) return;                                    // logger 自身
    if (level < Registry::Instance()->GetGlobalLevel()) return;       // 全局
    // ...
}
```

`SetGlobalLevel(Off)` 一键静音所有 Logger，恢复时各 Logger 自己的 level 配置不丢。`atomic<Level>` 用 `memory_order_relaxed` 读——读热路径每条日志只多一次原子 load，x86 上和普通 load 一样便宜。

## 性能数据

环境：MinGW GCC 13.1，**Release 模式**，1 万条日志业务线程总耗时。

| Sink / 路径 | 总耗时 | 单条 |
|---|---|---|
| 被过滤的日志（early return） | 58 us | **0.01 us** |
| CallbackSink（无 IO） | 411 us | 0.04 us |
| **LOG_INFO_FMT_TO（std::format）** | **2.25 ms** | **0.23 us** |
| ConsoleSinkMT（重定向 cout） | 7.87 ms | 0.79 us |
| LOG_INFO_TO（string +） | 8.87 ms | 0.89 us |
| ConsoleSinkST（重定向 cout） | 9.28 ms | 0.93 us |
| FileSinkMT（同步写盘） | 135 ms | 13.52 us |
| AsyncLogger + FileSinkMT（业务入队） | 175 ms | 17.55 us |
| RotatingFileSinkMT（含滚动） | 188 ms | 18.76 us |

### 几个值得讲的发现

**1. `std::format` 比字符串拼接快 4 倍（0.23 us vs 0.89 us）**

```cpp
LOG_INFO_TO(lg, "user=" + name + " age=" + std::to_string(age));   // 0.89 us
LOG_INFO_FMT_TO(lg, "user={} age={}", name, age);                   // 0.23 us
```

字符串拼接每次会触发多次堆分配 + `std::to_string` 构造。`std::format` 在 C++20 下编译期推导参数类型，写入预分配的小缓冲，单次堆分配。Release 优化让两者差距从 Debug 的 1.3x 拉大到 4x。

**2. 双层过滤的 early return 路径几乎免费（0.01 us）**

`SetGlobalLevel(Off)` 之后所有日志走早返回路径——比一次 `if + atomic load` 还便宜，编译器把整个函数 inline 后只剩两个比较和一个跳转。

**3. AsyncLogger 在本地磁盘场景下并不显著快于同步**

```
FileSinkMT (sync)               13.52 us
AsyncLogger + FileSinkMT        17.55 us   ← 反而慢 30%
```

这是个**真实的性能特征，不是 bug**——异步路径的固有开销包括：

- 每条日志构造 `Record` + 拷贝 `sinks_` 快照
- lambda 捕获（heap 分配 + 引用计数）
- ThreadPool 入队 + 条件变量唤醒
- 后台 worker 出队执行

而 OS 对本地文件 IO 已经做了 page cache + write-behind 优化，同步 `ofstream::write` 在大多数情况下是**纯内存拷贝**（不会真触发磁盘）——这种场景下异步反而成了纯开销。

**异步的优势场景**：

- 真正阻塞的远程 IO（HTTP 上报、Kafka producer、网络 syslog）
- 慢盘/网盘场景（NFS、SMB、机械盘满）
- 业务线程对延迟极敏感（高频交易、游戏帧线程）

这些场景下 IO 一次几百毫秒到几秒，异步把"等 IO"的时间从业务线程上彻底挪开，加速比能拉到 100x+。

**4. ConsoleSinkMT 比 ConsoleSinkST 略快（0.79 vs 0.93 us）**

反直觉——多线程版竟然比单线程版快？  
原因：std::cout 在标准库实现里**内部已经有同步**，`ConsoleSinkMT` 加的 `std::mutex` 和 std::cout 内部的锁刚好被合并优化。`ConsoleSinkST` 用 `NullMutex` 后，编译器看不到锁的提示，反而少了一些指令重排的优化机会。差距很小（0.14 us），不是热点。

## 项目阶段

按 [`docs/架构指导.md`](docs/架构指导.md) 分 9 个 phase 迭代：

| Phase | 主题 |
|---|---|
| P0 | 工程结构 + 命名空间 |
| P1 | 控制台最小闭环 |
| P2 | 文件 + 多 Sink |
| P3 | 等级过滤 |
| P4 | SourceLocation 结构化 |
| P5 | Stylizer + BaseSink |
| P6 | 滚动 + 模板锁（NullMutex） |
| P7 | 多 Logger + Registry + 全局配置 |
| P8 | 异步日志 + ThreadPool |
| P9 | ColorSink / CallbackSink / std::format |

每个 phase 一个 commit 节点，git log 可追溯每个设计决策。

## License

MIT
