#pragma once
namespace logging{
    /**
 * @brief 拆分源 file/line/func
 */
    struct SourceLocation {
        const char* file;
        int line;
        const char* function;
    };
}
