//
// Created by adminstrator on 2026/5/15.
//

#ifndef TINYLOGLIBRARY_LOGGER_H
#define TINYLOGLIBRARY_LOGGER_H

enum class LogLevel {
    Debug,  // 调试信息
    Info,   // 一般信息
    Warn,   // 警告
    Error,  //错误
    Critical// 严重错误
};


/**
 * @brief 日志器
 */
class Logger {
public:
    Logger();

    /*
     * 入口函数的设计，需要支持
     */
    // 入口函数
    void Debug() {

    }

private:
};


#endif //TINYLOGLIBRARY_LOGGER_H
