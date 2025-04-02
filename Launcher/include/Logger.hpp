#pragma once

#include <string>
#include <windows.h>
#include <format>
#include <iostream>

class Logger {
public:
    static void Log(const std::wstring& message) {
        SYSTEMTIME time;
        GetLocalTime(&time);
        std::wcout << std::format(L"[{:02d}:{:02d}:{:02d}] {}",
                                 time.wHour, time.wMinute, time.wSecond,
                                 message) << std::endl;
    }

    static void LogError(const std::wstring& message) {
        SYSTEMTIME time;
        GetLocalTime(&time);
        std::wcerr << std::format(L"[{:02d}:{:02d}:{:02d}] [ERROR] {}",
                                 time.wHour, time.wMinute, time.wSecond,
                                 message) << std::endl;
    }
}; 