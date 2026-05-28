#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "FullFormatter.h"
#include "Sink.h"

namespace logging {

/**
 * @brief 文件输出 Sink
 *
 * 设计要点：
 *   1. 路径由调用方传入，避免依赖 CWD
 *   2. 构造时打开一次，析构时关闭——不要每条日志都开关文件
 *   3. std::ios::app 追加写
 *   4. SinkIt 只 '\n'，刷盘交给 Flush()
 *   5. 自动建父目录
 *   6. 内部错误走 std::cerr，绝不走 LOG_ERROR（会无限递归）
 */
class FileSink : public BaseSink {
public:
    explicit FileSink(std::string path)
        : path_(std::move(path)) {
        const std::filesystem::path p(path_);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }

        ofs_.open(path_, std::ios::out | std::ios::app);
        formatter_ = std::make_unique<FullFormatter>();

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

    void SinkIt(const Record& record) override {
        if (!ofs_.is_open()) {
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

} // namespace logging
