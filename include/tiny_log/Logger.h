#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Level.h"
#include "Sink.h"

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

    void Print(Level level, const std::string& message);
    void Print(Level level,
               const std::string& message,
               const std::string& source_location);

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

private:
    std::vector<std::shared_ptr<Sink>> sinks_;

    Level level_ = Level::Info;
};

} // namespace logging
