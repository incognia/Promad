#include "ConfigManager.hpp"
#include "ProcessManager.hpp"
#include "BrowserDetector.hpp"
#include "ProfileManager.hpp"
#include "CacheCleaner.hpp"
#include "BrowserLauncher.hpp"
#include "Logger.hpp"

int wmain() {
    // Cargar configuración
    auto& config = ConfigManager::getInstance();
    if (!config.loadConfig()) {
        Logger::LogError(L"No se pudo cargar la configuración. Usando valores por defecto.");
    }

    Logger::Log(L"Iniciando Browser Manager (Versión Híbrida Mejorada)");
    
    try {
        // 1. Cerrar todos los navegadores
        ProcessManager::TerminateBrowserProcesses();

        // 2. Detectar navegador predeterminado
        auto browserInfo = BrowserDetector::DetectDefaultBrowser();
        Logger::Log(std::format(L"Navegador predeterminado: {} ({})", 
                              browserInfo.name, 
                              browserInfo.isStoreInstalled ? "Store" : "EXE"));

        // 3. Obtener perfil del usuario
        auto profilePath = ProfileManager::GetProfilePath(browserInfo);
        
        // 4. Limpiar caché
        if (!profilePath.empty()) {
            CacheCleaner::CleanBrowserCache(browserInfo, profilePath);
        } else {
            Logger::LogError(L"No se pudo determinar la ruta del perfil - omitiendo limpieza de caché");
        }

        // 5. Abrir URL en el navegador predeterminado
        if (!BrowserLauncher::OpenDefaultBrowser(config.getDefaultUrl())) {
            return 1;
        }

        Logger::Log(L"Operación completada exitosamente");
        return 0;
    } 
    catch (const std::exception& e) {
        Logger::LogError(std::format(L"Error inesperado: {}", e.what()));
        return 1;
    }
}
