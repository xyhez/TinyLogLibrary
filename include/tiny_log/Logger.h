#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Level.h"
#include "Sink.h"
#include "SourceLocation.h"

namespace logging {

/**
 * @brief 日志器：管判断和分发
 *
 * Logger 不关心格式、不关心目的地——那是 Formatter 和 Sink 的事。
 * 它只做：构造 Record → 遍历 sinks → 调 SinkIt。
 */
class Logger {
public:
    Logger() = default;
    Logger(std::string  name) : name_(std::move(name)) {

    }

    void Print(Level level, const std::string& message);
    void Print(Level level,
               const std::string& message,
               SourceLocation source_location);

    void AddSink(std::shared_ptr<Sink> sink);
    void ClearSinks();
    void Flush();
    void SetLevel(const Level level) {
        level_ = level;
    }
    [[nodiscard]] Level GetLevel() const {
        return level_;
    }
    [[nodiscard]] inline bool ShouldLog(Level level) const {
        return level >= level_;
    }

    void SetFlushOnLevel(const Level level) {
        flush_on_level_ = level;
    }
    [[nodiscard]] Level GetFlushOnLevel() const {
        return flush_on_level_;
    }

private:
    std::vector<std::shared_ptr<Sink>> sinks_;

    Level level_ = Level::Info;
    Level flush_on_level_ = Level::Critical;
    std::string name_;
};

} // namespace logging
