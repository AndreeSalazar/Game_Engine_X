cmake_minimum_required(VERSION 3.15)
project(DirectX12Test LANGUAGES CXX)

# Configuración del compilador C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración para Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

# Directorios - Estructura estilo Unreal Engine 5
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Engine/Core/Source)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Engine/Core/Include)
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/Engine/Rendering/Shaders)
set(CONTENT_DIR ${CMAKE_SOURCE_DIR}/Engine/Content)
set(CONFIG_DIR ${CMAKE_SOURCE_DIR}/Engine/Config)

# Archivos fuente
file(GLOB_RECURSE SOURCES 
    "${SOURCE_DIR}/*.cpp"
)

# Archivos de encabezado (no necesarios en add_executable, pero útiles para IDE)
file(GLOB_RECURSE HEADERS 
    "${INCLUDE_DIR}/*.h"
)

# Crear ejecutable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Incluir directorios
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# Enlazar bibliotecas de DirectX 12
target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d12
    dxgi
    d3dcompiler
)

# Configuración para Debug
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

# Configuración de compilación
if(MSVC)
    # Deshabilitar warnings específicos de MSVC
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
    )
endif()

# Copiar shaders al directorio de salida después de la compilación
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    COMMENT "Copiando shaders al directorio de salida..."
)

# Copiar archivos de configuración
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_DIR}/Engine.ini $<TARGET_FILE_DIR:${PROJECT_NAME}>/Engine.ini
    COMMENT "Copiando archivos de configuración..."
)

