#include "tiny_log/AsyncLogger.h"

void logging::AsyncLogger::Print(Level level,
                 const std::string& message,
                 SourceLocation source_location) {
    // 双层过滤在入队之前做
    if (!ShouldLog(level))
        return;
    if (level < detail::Registry::GetInstance()->GetGlobalLevel())
        return;
    Record record(level, message,std::chrono::system_clock::now(),source_location);

    auto sinks_snapshot = sinks_;
    Level flush_level = flush_on_level_;

    pool_->SubmitTask([record = std::move(record),
    sinks =std::move(sinks_snapshot), flush_level]() mutable {
        for (auto& sink : sinks) {
            sink->SinkIt(record);
        }
        if (record.GetLevel() >= flush_level) {
            for (auto& sink : sinks) {
                sink->Flush();
            }
        }
    });
}
