#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <format>
#include <shlobj.h>
#include <wil/resource.h>
#include "Logger.hpp"
#include "BrowserDetector.hpp"

namespace fs = std::filesystem;

class ProfileManager {
public:
    static std::wstring GetProfilePath(const BrowserDetector::BrowserInfo& browserInfo) {
        wil::unique_cotaskmem_string localAppData;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &localAppData))) {
            Logger::LogError(L"No se pudo obtener la ruta LocalAppData");
            return L"";
        }

        if (browserInfo.name == L"chrome") {
            return FindChromeEdgeProfile(localAppData.get(), L"Google\\Chrome\\User Data");
        } 
        else if (browserInfo.name == L"edge") {
            return FindChromeEdgeProfile(localAppData.get(), L"Microsoft\\Edge\\User Data");
        } 
        else { // Firefox
            return FindFirefoxProfile();
        }
    }

private:
    static std::wstring FindChromeEdgeProfile(const wchar_t* localAppData, const std::wstring& basePath) {
        std::vector<std::wstring> profilePaths = {
            std::format(L"{}\\{}\\Default", localAppData, basePath),
            std::format(L"{}\\{}\\Profile 1", localAppData, basePath),
            std::format(L"{}\\{}\\Default User", localAppData, basePath)
        };

        for (const auto& path : profilePaths) {
            if (fs::exists(path)) {
                Logger::Log(std::format(L"Perfil encontrado en: {}", path));
                return path;
            }
        }

        Logger::LogError(L"No se encontró un perfil válido");
        return L"";
    }

    static std::wstring FindFirefoxProfile() {
        wil::unique_cotaskmem_string appData;
        if (FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appData))) {
            Logger::LogError(L"No se pudo obtener la ruta AppData");
            return L"";
        }

        fs::path profilesDir = fs::path(appData.get()) / L"Mozilla" / L"Firefox" / L"Profiles";
        
        // Orden de prioridad para perfiles de Firefox
        const std::vector<std::wstring> profilePriority = {
            L".default-release",  // Perfil moderno
            L".default",          // Perfil tradicional
            L"*.dev-edition-default" // Edición desarrollador
        };
        
        for (const auto& profile : profilePriority) {
            for (const auto& entry : fs::directory_iterator(profilesDir)) {
                if (entry.is_directory()) {
                    std::wstring dirName = entry.path().filename().wstring();
                    if (dirName.find(profile) != std::wstring::npos) {
                        Logger::Log(std::format(L"Perfil Firefox encontrado: {}", dirName));
                        return entry.path().wstring();
                    }
                }
            }
        }
        
        // Fallback: primer perfil encontrado
        for (const auto& entry : fs::directory_iterator(profilesDir)) {
            if (entry.is_directory()) {
                Logger::Log(std::format(L"Usando perfil Firefox encontrado: {}", 
                                      entry.path().filename().wstring()));
                return entry.path().wstring();
            }
        }
        
        Logger::LogError(L"No se encontraron perfiles de Firefox");
        return L"";
    }
}; 