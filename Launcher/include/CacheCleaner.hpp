#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <format>
#include "Logger.hpp"
#include "BrowserDetector.hpp"

namespace fs = std::filesystem;

class CacheCleaner {
public:
    static void CleanBrowserCache(const BrowserDetector::BrowserInfo& browserInfo, const std::wstring& profilePath) {
        if (profilePath.empty()) {
            Logger::LogError(L"No se puede limpiar caché - ruta de perfil vacía");
            return;
        }

        Logger::Log(L"Iniciando limpieza de caché...");
        
        std::vector<fs::path> cachePaths = GetCachePaths(browserInfo.name, profilePath);
        size_t deletedCount = 0;

        for (const auto& path : cachePaths) {
            if (CleanPath(path)) {
                deletedCount++;
            }
        }

        Logger::Log(std::format(L"Limpieza completada. {} ubicaciones limpiadas", deletedCount));
    }

private:
    static std::vector<fs::path> GetCachePaths(const std::wstring& browserName, const std::wstring& profilePath) {
        if (browserName == L"chrome" || browserName == L"edge") {
            return {
                fs::path(profilePath) / "Cache",
                fs::path(profilePath) / "Code Cache",
                fs::path(profilePath) / "GPUCache",
                fs::path(profilePath) / "Media Cache",
                fs::path(profilePath) / "CacheStorage",
                fs::path(profilePath) / "Service Worker"
            };
        }
        else { // Firefox
            return {
                fs::path(profilePath) / "cache2",
                fs::path(profilePath) / "startupCache",
                fs::path(profilePath) / "thumbnails",
                fs::path(profilePath) / "crashes",
                fs::path(profilePath) / "storage" / "default" / "cache"
            };
        }
    }

    static bool CleanPath(const fs::path& path) {
        try {
            if (fs::exists(path)) {
                Logger::Log(std::format(L"Eliminando: {}", path.wstring()));
                fs::remove_all(path);
                
                // Verificación post-eliminación
                if (!fs::exists(path)) {
                    Logger::Log(std::format(L"Eliminación exitosa: {}", path.wstring()));
                    return true;
                }
                Logger::LogError(std::format(L"No se pudo eliminar completamente: {}", path.wstring()));
            }
            return false;
        } 
        catch (const fs::filesystem_error& e) {
            Logger::LogError(std::format(L"Error al eliminar {}: {}", path.wstring(), e.what()));
            return false;
        }
    }
}; 