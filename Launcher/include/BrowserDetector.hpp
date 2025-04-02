#pragma once

#include <windows.h>
#include <shlobj.h>
#include <string>
#include <wil/resource.h>
#include "Logger.hpp"

class BrowserDetector {
public:
    struct BrowserInfo {
        std::wstring name;
        std::wstring executable;
        bool isStoreInstalled;
    };

    static BrowserInfo DetectDefaultBrowser() {
        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;
        
        if (SUCCEEDED(AssocQueryStringW(
            ASSOCF_NONE, 
            ASSOCSTR_EXECUTABLE, 
            L"http", 
            L"open", 
            path, 
            &size
        ))) {
            std::wstring browserPath(path);
            bool isStoreApp = IsInstalledFromMicrosoftStore(browserPath);

            if (browserPath.find(L"chrome.exe") != std::wstring::npos) {
                Logger::Log(isStoreApp ? L"Chrome detectado (Microsoft Store)" : L"Chrome detectado (Instalación tradicional)");
                return { L"chrome", L"chrome.exe", isStoreApp };
            }
            if (browserPath.find(L"msedge.exe") != std::wstring::npos) {
                Logger::Log(isStoreApp ? L"Edge detectado (Microsoft Store)" : L"Edge detectado (Instalación tradicional)");
                return { L"edge", L"msedge.exe", isStoreApp };
            }
            if (browserPath.find(L"firefox.exe") != std::wstring::npos) {
                Logger::Log(isStoreApp ? L"Firefox detectado (Microsoft Store)" : L"Firefox detectado (Instalación tradicional)");
                return { L"firefox", L"firefox.exe", isStoreApp };
            }
        }
        
        Logger::Log(L"Navegador no identificado, usando Edge como predeterminado");
        return { L"edge", L"msedge.exe", false };
    }

private:
    static bool IsInstalledFromMicrosoftStore(const std::wstring& browserPath) {
        // Método 1: Verificar ruta típica de WindowsApps
        if (browserPath.find(L"WindowsApps") != std::wstring::npos) {
            return true;
        }
        
        // Método 2: Verificar registro para apps UWP
        wil::unique_hkey hKey;
        std::wstring regPath = L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppModel\\Repository\\Packages";
        
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t packageName[256];
            DWORD index = 0;
            
            while (RegEnumKeyW(hKey.get(), index++, packageName, ARRAYSIZE(packageName)) == ERROR_SUCCESS) {
                if (wcsstr(packageName, L"MicrosoftEdge") || 
                    wcsstr(packageName, L"Google.Chrome") || 
                    wcsstr(packageName, L"Mozilla.Firefox")) {
                    return true;
                }
            }
        }
        
        return false;
    }
}; 