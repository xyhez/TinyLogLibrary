#include <chrono>
#include <utility>

#include "tiny_log/Record.h"
#include "tiny_log/Logger.h"

namespace logging {

void Logger::Print(Level level,
                   const std::string& message,
                   SourceLocation sourceLocation) {
    if (!ShouldLog(level)) {
        return;
    }
    Record record(level,
                  message,
                  std::chrono::system_clock::now(),
                  sourceLocation);
    for (auto& sink : sinks_) {
        sink->SinkIt(record);
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
