#pragma once

#include "webgpu-headers/webgpu.h"
#include "utils.h"
#include "wgpu.h"

#include <GLFW/glfw3.h>

//https://eliemichel.github.io/LearnWebGPU/getting-started/hello-webgpu.html
// Add this in order to remain compatible with Dawn
#define wgpuInstanceRelease wgpuInstanceDrop

namespace WGPUEnv {

    struct sInstance {
        WGPUInstance instance;
        WGPUAdapter  adapter;
        WGPUSurface  surface;


        // Methods =========================
        void initialize(GLFWwindow *window, void* callback);

        void clean();

        
        // Events =========================
        static void e_adapter_request_ended(WGPURequestAdapterStatus status, 
                                            WGPUAdapter adapter, 
                                            char const* message, 
                                            void* user_data);
    };


    struct sPayload {
        sInstance  *wgpu_man_instance = NULL; 
        GLFWwindow *window = NULL;
        void *callback = NULL;
    };
};