#include <chrono>
#include <utility>

#include "tiny_log/Record.h"
#include "tiny_log/Logger.h"
#include "tiny_log/detail/Registry.h"

namespace logging {

void Logger::Print(Level level,
                   const std::string& message,
                   SourceLocation sourceLocation) {
    // 双层过滤：先查 logger 自己的 level，再查全局 level
    if (!ShouldLog(level)) {
        return;
    }
    if (level < detail::Registry::GetInstance()->GetGlobalLevel()) {
        return;
    }

    Record record(level,
                  message,
                  std::chrono::system_clock::now(),
                  sourceLocation);
    for (auto& sink : sinks_) {
        sink->SinkIt(record);
    }
    if (level >= flush_on_level_) {
        Flush();
    }
}

void Logger::Print(Level level, const std::string& message) {
    Print(level, message, SourceLocation());
}

void Logger::AddSink(std::shared_ptr<Sink> sink) {
    sinks_.push_back(std::move(sink));
}

void Logger::ClearSinks() {
    sinks_.clear();
}

void Logger::Flush() {
    for (auto& sink : sinks_) {
        sink->Flush();
    }
}

} // namespace logging
