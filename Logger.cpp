#include "Logger.h"

void Logger::Print(LogLevel level,
                   const std::string& message,
                   const std::string& source_location) {

    LogRecord record(level,
                     message,
                     std::chrono::system_clock::now(),
                     source_location);
    for (auto& sink : sinks_) {
        sink->SinkIt(record);
    }
}

void Logger::Print(LogLevel level, const std::string& message) {
    // 没有源码位置时转发给三参版本，避免逻辑重复
    Print(level, message, "");
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
