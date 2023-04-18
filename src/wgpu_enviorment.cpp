#include "wgpu_enviorment.h"
#include "webgpu-headers/webgpu.h"
#include "wgpu.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windef.h>


WGPUSurface get_surface(GLFWwindow *window, WGPUInstance instance) {
#if WGPU_TARGET == WGPU_TARGET_WINDOWS
    HWND hwnd = glfwGetWin32Window(window);
    HINSTANCE hinstance = GetModuleHandle(NULL);

    const WGPUSurfaceDescriptorFromWindowsHWND chained = (WGPUSurfaceDescriptorFromWindowsHWND){
                    .chain =
                        (WGPUChainedStruct){
                            .next = NULL,
                            .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
                        },
                    .hinstance = hinstance,
                    .hwnd = hwnd,
                };
    
    WGPUSurfaceDescriptor descriptor = {
            .nextInChain =
                (const WGPUChainedStruct *)& chained,
                .label = NULL,
    };

    WGPUSurface surface = wgpuInstanceCreateSurface(instance, 
                                                    &descriptor);
#endif
    assert_msg(!surface, "Error creating surface");

    return surface;
}

void WGPUEnv::sInstance::initialize(GLFWwindow *window, void *callback) {
    // Create the payload that stores the data during the async generation
    sPayload payload = {
        .wgpu_man_instance = this,
        .window = window, 
        .callback = callback
    };

    // Create instance
    {
        WGPUInstanceDescriptor instance_descr = {
            .nextInChain = NULL,
        };
        instance = wgpuCreateInstance(&instance_descr);

        assert_msg(!instance, 
                   "Error creating WebGPU instance");
    }

    // Fetch the render surface
    {
        surface = get_surface(window, instance);
    }

    // Request adapter
    {
        WGPURequestAdapterOptions options = {
            .nextInChain = NULL,
            .compatibleSurface = surface
        };
        wgpuInstanceRequestAdapter(instance, 
                                   &options, 
                                   e_adapter_request_ended,
                                   (void *)&payload);
    }
}

void WGPUEnv::sInstance::clean() {
    wgpuInstanceRelease(instance);
    wgpuAdapterDrop(adapter);
}

// Events =========================
void  WGPUEnv::sInstance::e_adapter_request_ended(WGPURequestAdapterStatus status, 
                                                  WGPUAdapter adapter, 
                                                  char const* message, 
                                                  void* user_data) {
    assert_msg(status != WGPURequestAdapterStatus_Success, "Error loading adapter");
    ((sPayload*) user_data)->wgpu_man_instance->adapter = adapter;
    
    // Inspect adapter features
    uint32_t function_count = wgpuAdapterEnumerateFeatures(adapter, NULL);

    WGPUFeatureName* features = (WGPUFeatureName*) malloc(sizeof(WGPUFeatureName) * function_count);
    
    for(uint32_t i = 0; i < function_count; i++) {
        std::cout << features[i] << std::endl;
    }
}