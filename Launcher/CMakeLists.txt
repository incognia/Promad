cmake_minimum_required(VERSION 3.15)
project(BrowserManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Encontrar nlohmann/json
find_package(nlohmann_json 3.11.2 REQUIRED)

# Añadir ejecutable
add_executable(browser_manager
    src/Launcher.cpp
    src/main.cpp
)

# Incluir directorios
target_include_directories(browser_manager PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Enlazar bibliotecas
target_link_libraries(browser_manager PRIVATE
    nlohmann_json::nlohmann_json
)

# Copiar archivo de configuración
file(COPY ${CMAKE_SOURCE_DIR}/config/default_config.json
     DESTINATION ${CMAKE_BINARY_DIR})
