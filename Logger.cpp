#include "Logger.h"


void Logger::Print(LogLevel level, const std::string &message) {
    for (auto sink : sinks_) {
        sink->SinkIt(LogRecord(level, message,std::chrono::system_clock::now(),"log source"));
    }
}

void Logger::AddSink(std::shared_ptr<Sink> sink) {
    sinks_.push_back(sink);
}

void Logger::ClearSinks() {

}

void Logger::Flush() {
    for (auto sink : sinks_) {
        sink->Flush();
    }
}
