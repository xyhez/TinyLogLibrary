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
)
