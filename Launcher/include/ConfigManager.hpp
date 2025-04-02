#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>

namespace fs = std::filesystem;
using json = nlohmann::json;

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    bool loadConfig(const fs::path& configPath = "config/default_config.json") {
        try {
            std::ifstream file(configPath);
            if (!file.is_open()) {
                Logger::LogError(std::format(L"No se pudo abrir el archivo de configuración: {}", configPath.wstring()));
                return false;
            }
            config = json::parse(file);
            return true;
        } catch (const std::exception& e) {
            Logger::LogError(std::format(L"Error al cargar la configuración: {}", std::wstring(e.what(), e.what() + strlen(e.what()))));
            return false;
        }
    }

    // Getters para la configuración
    std::wstring getDefaultUrl() const {
        return std::wstring(config["browser"]["default_url"].get<std::string>().begin(),
                          config["browser"]["default_url"].get<std::string>().end());
    }

    int getProcessTerminationAttempts() const {
        return config["browser"]["process_termination_attempts"].get<int>();
    }

    int getProcessTerminationDelay() const {
        return config["browser"]["process_termination_delay_ms"].get<int>();
    }

    std::wstring getLogFilePath() const {
        return std::wstring(config["logging"]["file_path"].get<std::string>().begin(),
                          config["logging"]["file_path"].get<std::string>().end());
    }

    bool getConsoleOutput() const {
        return config["logging"]["console_output"].get<bool>();
    }

    std::vector<std::wstring> getChromeProfilePaths() const {
        std::vector<std::wstring> paths;
        for (const auto& path : config["profiles"]["chrome"]["profile_paths"]) {
            paths.emplace_back(path.get<std::string>().begin(), path.get<std::string>().end());
        }
        return paths;
    }

    std::wstring getChromeBasePath() const {
        return std::wstring(config["profiles"]["chrome"]["base_path"].get<std::string>().begin(),
                          config["profiles"]["chrome"]["base_path"].get<std::string>().end());
    }

    bool getCleanupOnExit() const {
        return config["cache"]["cleanup_on_exit"].get<bool>();
    }

    bool getIncludeCookies() const {
        return config["cache"]["include_cookies"].get<bool>();
    }

    bool getIncludeHistory() const {
        return config["cache"]["include_history"].get<bool>();
    }

private:
    ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    json config;
}; 