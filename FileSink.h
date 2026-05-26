#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "Sink.h"

/**
 * @brief 文件输出 Sink：把日志写到指定路径
 *
 * 设计要点：
 *   1. 路径由调用方传入，FileSink 不再写死路径——更通用，也避免依赖 CWD
 *   2. 把 ofstream 做成成员，避免每条日志开关一次文件
 *   3. 用 std::ios::app 追加写，不要 trunc——否则后写的日志会覆盖先写的
 *   4. SinkIt 末尾 '\n' 即可，不要 std::endl —— endl 会强制 flush，频繁刷盘性能差
 *      真要刷盘走 Flush() 接口，由调用方决定时机
 *   5. 构造时用 std::filesystem::create_directories 自动建父目录
 *   6. 内部错误一律走 std::cerr，不走 LOG_ERROR——日志组件不能依赖自己上报错误
 */
class FileSink : public Sink {
public:
    /**
     * @brief 构造一个写到 path 的 FileSink
     * @param path 目标文件路径（绝对/相对均可），父目录不存在会自动创建
     */
    explicit FileSink(std::string path)
        : path_(std::move(path)) {
        // 1. 父目录不存在则递归创建（已存在 no-op）
        const std::filesystem::path p(path_);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }

        // 2. 追加模式打开
        ofs_.open(path_, std::ios::out | std::ios::app);

        // 3. 默认 formatter
        formatter_ = std::make_shared<FullFormatter>();

        // 4. 打开失败立刻吼出来，避免后续 silent failure
        if (!ofs_.is_open()) {
            std::cerr << "[FileSink] failed to open " << path_ << '\n';
        }
    }

    ~FileSink() override {
        if (ofs_.is_open()) {
            ofs_.flush();
            ofs_.close();
        }
    }

    void SinkIt(const LogRecord& record) override {
        if (!ofs_.is_open()) {
            // 只在第一次失败时报告，避免 1000 条日志喷 1000 行错误
            // 注意：这里绝对不能用 LOG_ERROR，否则会无限递归回到 SinkIt
            static bool reported = false;
            if (!reported) {
                std::cerr << "[FileSink] " << path_ << " not open, dropping logs\n";
                reported = true;
            }
            return;
        }
        ofs_ << formatter_->Format(record) << '\n';
    }

    void Flush() override {
        if (ofs_.is_open()) {
            ofs_.flush();
        }
    }

private:
    std::string   path_;
    std::ofstream ofs_;
};
