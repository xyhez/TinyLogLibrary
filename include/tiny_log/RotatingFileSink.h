#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>

#include "FullFormatter.h"
#include "Sink.h"
#include "detail/NullMutex.h"

namespace logging {

/**
 * @brief 按文件大小滚动的文件 Sink，保留最近的 N 个旧文件
 *
 * 滚动策略：
 *   写入后若文件超过 max_size 字节，触发一次 Rotate：
 *     base.log    → base.1.log
 *     base.1.log  → base.2.log
 *     ...
 *     base.(N-1).log → base.N.log
 *     超过 max_files 的最老文件被删除
 *   然后重新打开 base.log 作为新文件继续写。
 *
 * 设计要点：
 *   1. 大小走内存计数（current_size_），启动时用 file_size 初始化一次，
 *      避免重启后追加丢精度。
 *   2. 重命名必须倒序——正向会覆盖目标文件。
 *   3. 文件名拼接：在扩展名前插数字（app.log → app.1.log），保留扩展名
 *      让外部工具（编辑器/日志查看器）能识别。
 *   4. 加锁交给 BaseSink<Mutex>，本类只实现 SinkItImpl/FlushImpl。
 *
 * @tparam Mutex 锁类型；单线程用 NullMutex 零开销，多线程用 std::mutex
 */
template<typename Mutex>
class RotatingFileSink : public BaseSink<Mutex> {
    using BaseSink<Mutex>::formatter_;

public:
    RotatingFileSink(std::string base_path,
                     const std::size_t max_size,
                     const std::size_t max_files)
        : base_path_(std::move(base_path))
        , max_size_(max_size)
        , max_files_(max_files) {
        // 1. 自动建父目录
        const std::filesystem::path p(base_path_);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }

        // 2. 启动时问一次文件大小，让追加模式能继承现有进度
        std::error_code ec;
        if (std::filesystem::exists(base_path_, ec)) {
            const auto sz = std::filesystem::file_size(base_path_, ec);
            current_size_ = ec ? 0 : sz;
        }

        // 3. 追加模式打开
        ofs_.open(base_path_, std::ios::out | std::ios::app);
        formatter_ = std::make_unique<FullFormatter>();

        if (!ofs_.is_open()) {
            std::cerr << "[RotatingFileSink] failed to open " << base_path_ << '\n';
        }
    }

    ~RotatingFileSink() override {
        if (ofs_.is_open()) {
            ofs_.flush();
            ofs_.close();
        }
    }

protected:
    void SinkItImpl(const Record& record) override {
        if (!ofs_.is_open()) {
            static bool reported = false;
            if (!reported) {
                std::cerr << "[RotatingFileSink] " << base_path_
                          << " not open, dropping logs\n";
                reported = true;
            }
            return;
        }

        const std::string text = formatter_->Format(record);
        ofs_ << text << '\n';
        current_size_ += text.size() + 1;  // +1 for '\n'

        if (current_size_ >= max_size_) {
            Rotate();
        }
    }

    void FlushImpl() override {
        if (ofs_.is_open()) {
            ofs_.flush();
        }
    }

private:
    /**
     * @brief 在扩展名前插入索引：app.log + 2 → app.2.log
     *
     * index 为 0 时返回原始路径。
     */
    static std::string CalcFilename(const std::string& base, std::size_t index) {
        if (index == 0) return base;

        const std::filesystem::path p(base);
        const std::string stem = p.stem().string();         // "app"
        const std::string ext  = p.extension().string();    // ".log"
        const std::filesystem::path parent = p.parent_path();

        const std::string filename = stem + "." + std::to_string(index) + ext;
        return parent.empty() ? filename : (parent / filename).string();
    }

    /**
     * @brief 执行一次倒序滚动 + 重新打开
     *
     * 顺序很关键：必须从最大索引往下重命名，否则后写的会覆盖前一个的目标。
     */
    void Rotate() {
        // 1. 关闭当前 ofs（Windows 上未关闭的文件不能被 rename/remove）
        ofs_.close();

        // 2. 倒序重命名：
        //    base.(max_files_-1).log → base.max_files_.log
        //    base.(max_files_-2).log → base.(max_files_-1).log
        //    ...
        //    base.log → base.1.log
        std::error_code ec;
        for (std::size_t i = max_files_; i > 0; --i) {
            const std::string src = CalcFilename(base_path_, i - 1);
            const std::string dst = CalcFilename(base_path_, i);

            if (!std::filesystem::exists(src, ec)) {
                continue;
            }
            // 目标若已存在先删（rename 在某些平台不会自动覆盖）
            std::filesystem::remove(dst, ec);
            std::filesystem::rename(src, dst, ec);
            if (ec) {
                std::cerr << "[RotatingFileSink] rename " << src << " -> " << dst
                          << " failed: " << ec.message() << '\n';
                ec.clear();
            }
        }

        // 3. 删除超出 max_files_ 的最老文件
        const std::string oldest = CalcFilename(base_path_, max_files_ + 1);
        std::filesystem::remove(oldest, ec);

        // 4. 重新打开 base.log，重置计数
        ofs_.open(base_path_, std::ios::out | std::ios::trunc);
        current_size_ = 0;

        if (!ofs_.is_open()) {
            std::cerr << "[RotatingFileSink] failed to reopen " << base_path_
                      << " after rotate\n";
        }
    }

    std::string   base_path_;   ///< 要写如的文件
    std::size_t   max_size_;    ///<
    std::size_t   max_files_;
    std::size_t   current_size_ = 0;
    std::ofstream ofs_;
};

using RotatingFileSinkST = RotatingFileSink<NullMutex>;
using RotatingFileSinkMT = RotatingFileSink<std::mutex>;

} // namespace logging
