#include <tiny_log/Logging.h>

int main() {
    LOG_TRACE("This is a Trace log.");
    LOG_DEBUG("This is a Debug log.");
    LOG_INFO("This is an Info log.");
    LOG_WARN("This is a Warn log.");
    LOG_ERROR("This is an Error log.");
    LOG_CRITICAL("This is a Critical log.");
    return 0;
}
