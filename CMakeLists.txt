cmake_minimum_required(VERSION 3.13)

SET(CMAKE_CXX_FLAGS  "-g")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT "wgpu-playground")

project(${PROJECT} LANGUAGES C CXX)

# PROJECT SOURCE
file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB CPP_SUBFOLDER_SOURCES "src/**/*.cpp")
file(GLOB H_SOURCES "src/*.h")
file(GLOB C_SOURCES "src/*.c")
file(GLOB C_SUBFOLDER_SOURCES "src/**/*.c")
set(includes_dir "includes/")

add_executable(${PROJECT} ${CPP_SOURCES} ${CPP_SUBFOLDER_SOURCES} ${C_SOURCES} ${C_SUBFOLDER_SOURCES} ${H_SOURCES})

# WGPU-NATIVE
if(MSVC)
    add_definitions(-DWGPU_TARGET=WGPU_TARGET_WINDOWS)
    target_compile_options(${PROJECT} PRIVATE /W4 /std:c++20)
    set(OS_LIBRARIES "userenv" "ws2_32" "Dwmapi" "dbghelp" "d3dcompiler" "D3D12" "D3D11" "DXGI" "setupapi" "Bcrypt" "ntdll")
elseif(APPLE)
    add_definitions(-DWGPU_TARGET=WGPU_TARGET_MACOS)
    set(OS_LIBRARIES "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")
    target_compile_options(${PROJECT} PRIVATE -x objective-c)
elseif(MSYS OR MINGW)
    add_definitions(-DWGPU_TARGET=WGPU_TARGET_WINDOWS)
    set(OS_LIBRARIES "userenv" "ws2_32" "Dwmapi" "dbghelp" "d3dcompiler" "D3D12" "D3D11" "DXGI" "setupapi" "Bcrypt" "ntdll")
else(MSVC)
if(USE_WAYLAND)
    add_definitions(-DWGPU_TARGET=WGPU_TARGET_LINUX_WAYLAND)
else(USE_WAYLAND)
    add_definitions(-DWGPU_TARGET=WGPU_TARGET_LINUX_X11)
endif(USE_WAYLAND)
    target_compile_options(${PROJECT} PRIVATE -Wall -Wextra -pedantic)
endif(MSVC)

add_subdirectory(webgpu)
target_include_directories(${PROJECT} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/WebGPU/include/webgpu")
target_link_libraries(${PROJECT} webgpu)

# GLM
add_subdirectory(glm)
target_link_libraries(${PROJECT} glm)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(${PROJECT} glfw)