#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace logging::detail {

/**
 * @brief 把 system_clock::time_point 格式化为 "YYYY-MM-DD HH:MM:SS.mmm" 字符串
 *
 * 跨平台处理：Windows 用 localtime_s，POSIX 用 localtime_r，避免 std::localtime
 * 返回的静态缓冲区在多线程下被竞争。
 */
inline std::string TimeToString(std::chrono::system_clock::time_point time) {
    const std::time_t t = std::chrono::system_clock::to_time_t(time);

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        time.time_since_epoch()) % 1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

} // namespace logging::detail
