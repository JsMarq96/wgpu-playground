#pragma once

#include <webgpu.h>
#include "utils.h"
#include "wgpu.h"

#include <GLFW/glfw3.h>

//https://eliemichel.github.io/LearnWebGPU/getting-started/hello-webgpu.html
// Add this in order to remain compatible with Dawn
#define wgpuInstanceRelease wgpuInstanceDrop

namespace WGPUEnv {

    struct sInstance {
        WGPUInstance            instance;
        WGPUAdapter             adapter;
        WGPUSurface             surface;
        WGPUDevice              device;
        WGPUQueue               device_queue;
        WGPUCommandEncoder      device_command_encoder;
        WGPUSwapChain           swapchain;

        bool                    is_initialized = false;
        WGPUTextureView         current_texture_view;

        // Methods =========================
        void initialize(GLFWwindow *window, void* callback);
        void clean();

        void render_frame();

        void _config_with_device();
        
        // Events =========================
        static void e_device_error(WGPUErrorType type, char const* message, void* user_data);
        static void e_adapter_request_ended(WGPURequestAdapterStatus status, 
                                            WGPUAdapter adapter, 
                                            char const* message, 
                                            void* user_data);
        static void e_device_request_ended(WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void *user_data);
    };


    struct sPayload {
        sInstance  *wgpu_man_instance = NULL; 
        GLFWwindow *window = NULL;
        void *callback = NULL;
    };
};