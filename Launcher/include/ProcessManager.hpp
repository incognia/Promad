#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <wil/resource.h>
#include <array>
#include <string_view>
#include "Logger.hpp"
#include "ConfigManager.hpp"

class ProcessManager {
public:
    static void TerminateBrowserProcesses() {
        Logger::Log(L"Iniciando cierre de procesos de navegadores...");
        
        wil::unique_handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
        if (!snapshot) {
            Logger::LogError(L"Error al crear snapshot de procesos");
            return;
        }

        PROCESSENTRY32W entry{.dwSize = sizeof(PROCESSENTRY32W)};
        
        if (Process32FirstW(snapshot.get(), &entry)) {
            do {
                for (const auto& process : BROWSER_PROCESSES) {
                    if (process == entry.szExeFile) {
                        TerminateProcessWithRetries(entry);
                        break;
                    }
                }
            } while (Process32NextW(snapshot.get(), &entry));
        }
    }

private:
    static constexpr std::array<std::wstring_view, 3> BROWSER_PROCESSES = {
        L"chrome.exe", 
        L"msedge.exe", 
        L"firefox.exe"
    };

    static void TerminateProcessWithRetries(const PROCESSENTRY32W& entry) {
        auto& config = ConfigManager::getInstance();
        int attempts = config.getProcessTerminationAttempts();
        int delay = config.getProcessTerminationDelay();

        for (int attempt = 0; attempt < attempts; ++attempt) {
            wil::unique_handle process_handle(OpenProcess(
                PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION,
                FALSE,
                entry.th32ProcessID
            ));
            
            if (process_handle) {
                Logger::Log(std::format(L"Cerrando proceso: {} (Intento {})", 
                                     entry.szExeFile, attempt + 1));
                
                if (TerminateProcess(process_handle.get(), 0)) {
                    WaitForSingleObject(process_handle.get(), 1000);
                    if (WaitForSingleObject(process_handle.get(), 0) == WAIT_OBJECT_0) {
                        Logger::Log(std::format(L"Proceso {} cerrado exitosamente", entry.szExeFile));
                        return;
                    }
                }
                
                if (attempt == attempts - 1) {
                    Logger::LogError(std::format(L"Fallo al cerrar {}", entry.szExeFile));
                }
            }
            Sleep(delay);
        }
    }
}; 