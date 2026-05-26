cmake_minimum_required(VERSION 3.16)

project(TinyLogLibrary
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(normaltest
    testcase/normaltest.cpp
        Logger.cpp
        Logger.h
        Sink.h
        LogLevel.h
        Logging.h
        ConsoleSink.h
        FileSink.h
        Formatter.h
        Tools.h
)

# 把项目根目录路径作为宏注入到编译器
# FileSink 用它拼绝对路径，避免依赖运行时的 CWD（CLion / 命令行 / 双击 exe 行为不一致）
target_compile_definitions(normaltest PRIVATE
    TINY_LOG_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
