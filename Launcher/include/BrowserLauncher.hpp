#pragma once

#include <string>
#include <windows.h>
#include <shellapi.h>
#include "Logger.hpp"

class BrowserLauncher {
public:
    static bool OpenDefaultBrowser(const std::wstring& url) {
        Logger::Log(std::format(L"Abriendo URL: {}", url));
        
        HINSTANCE result = ShellExecuteW(
            nullptr, 
            L"open", 
            url.c_str(), 
            nullptr, 
            nullptr, 
            SW_SHOWNORMAL
        );

        if (reinterpret_cast<intptr_t>(result) <= 32) {
            Logger::LogError(std::format(L"Error al abrir navegador. Código: {}", 
                                       reinterpret_cast<intptr_t>(result)));
            return false;
        }
        return true;
    }
}; 