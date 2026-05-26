#include <iostream>
#include "../Logging.h"

int main() {
    // 旧的 logging::Error("...") 函数入口已被废弃
    // C++17 下函数捕获不到调用点位置，所以入口改成宏

    LOG_TRACE("This is a Trace log.");
    LOG_DEBUG("This is a Debug log.");
    LOG_INFO("This is an Info log.");
    LOG_WARN("This is a Warn log.");
    LOG_ERROR("This is an Error log.");
    LOG_CRITICAL("This is a Critical log.");

    return 0;
}
