#pragma once
#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

/**
 * @brief 通用工具文件
 */

/**
 * @brief 把 system_clock::time_point 格式化为 "YYYY-MM-DD HH:MM:SS.mmm" 字符串
 *
 * 跨平台处理：Windows 用 localtime_s，POSIX 用 localtime_r，避免 std::localtime
 * 返回的静态缓冲区在多线程下被竞争。
 *
 * @param time 时间点（一般来自 LogRecord::GetTime()）
 * @return 本地时区格式化后的字符串
 */
inline std::string TimeToString(std::chrono::system_clock::time_point time) {
    // 1. 整秒部分 → time_t，用于 localtime
    const std::time_t t = std::chrono::system_clock::to_time_t(time);

    // 2. 毫秒尾巴：从 epoch 起的总毫秒数对 1000 取余
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        time.time_since_epoch()) % 1000;

    // 3. 转本地时间（线程安全版本）
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    // 4. 拼字符串：日期时间 + ".毫秒"
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}
