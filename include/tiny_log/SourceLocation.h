#pragma once

/**
 * @brief 拆分源 file/line/func
 */
struct SourceLocation {
    const char* file;
    const char* function;
    int line;
};