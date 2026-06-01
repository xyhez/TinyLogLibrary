cmake_minimum_required(VERSION 3.16)

project(TinyLogLibrary
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------- Library ----------------
add_library(tiny_log STATIC
    src/Logger.cpp
)

target_include_directories(tiny_log PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 把项目根注入为字符串宏，避免运行时依赖 CWD
target_compile_definitions(tiny_log PUBLIC
    TINY_LOG_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

# ---------------- Tests ----------------
add_executable(normaltest tests/normaltest.cpp
        include/tiny_log/detail/NullMutex.h
        include/tiny_log/RotatingFileSink.h
        include/tiny_log/detail/Registry.h
        include/tiny_log/src/Registry.cpp)
target_link_libraries(normaltest PRIVATE tiny_log)
