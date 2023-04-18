#include <webgpu-headers/webgpu.h>
#include <wgpu.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "utils.h"
#include "wgpu_enviorment.h"

#define WGPU_TARGET_MACOS 1
#define WGPU_TARGET_LINUX_X11 2
#define WGPU_TARGET_WINDOWS 3
#define WGPU_TARGET_LINUX_WAYLAND 4

#if WGPU_TARGET == WGPU_TARGET_MACOS
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

#include <GLFW/glfw3.h>
#if WGPU_TARGET == WGPU_TARGET_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#elif WGPU_TARGET == WGPU_TARGET_LINUX_X11
#define GLFW_EXPOSE_NATIVE_X11
#elif WGPU_TARGET == WGPU_TARGET_LINUX_WAYLAND
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif WGPU_TARGET == WGPU_TARGET_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

GLFWwindow* window = NULL;
WGPUEnv::sInstance wgpu_instance = {};

void main_render_loop() {
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    wgpu_instance.clean();
}

int main() {
    std::cout << "Hello" << std::endl;

    if(!glfwInit()) {
        // Quit
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(640, 480, "WegBPU", NULL, NULL);
    
    wgpu_instance.initialize(window, (void*) main_render_loop);
    

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}