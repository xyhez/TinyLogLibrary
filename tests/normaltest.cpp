#include <tiny_log/Logging.h>

int main() {
    LOG_TRACE("NO.");
    LOG_DEBUG("NO.");
    LOG_INFO("YES");
    LOG_WARN("YES");
    LOG_ERROR("YES");
    LOG_CRITICAL("YES");
    return 0;
}
