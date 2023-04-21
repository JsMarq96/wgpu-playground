#include "wgpu_enviorment.h"
#include "webgpu.h"
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

    const WGPUSurfaceDescriptorFromWindowsHWND chained = {
                    .chain = {
                            .next = NULL,
                            .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
                        },
                    .hinstance = hinstance,
                    .hwnd = hwnd,
                };
    
    WGPUSurfaceDescriptor descriptor = {
            .nextInChain =
                (const WGPUChainedStruct *)&chained,
                .label = NULL,
    };

    WGPUSurface surface = wgpuInstanceCreateSurface(instance, 
                                                    &descriptor);
#endif
    assert_msg(surface, "Error creating surface");

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

        assert_msg(instance, 
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

 void WGPUEnv::sInstance::_config_with_device() {
    // Create the Device Queue
    {
        device_queue = wgpuDeviceGetQueue(device);
    }

    // Create Swapchain
    {
        WGPUSwapChainDescriptor swapchain_descr = {
            .nextInChain = NULL,
            .usage = WGPUTextureUsage_RenderAttachment,
            .format = wgpuSurfaceGetPreferredFormat(surface, adapter),
            .width = 640,
            .height = 480,
            .presentMode = WGPUPresentMode_Fifo
        };

        swapchain = wgpuDeviceCreateSwapChain(device, surface, &swapchain_descr);
    }

    is_initialized = true;
 }

void WGPUEnv::sInstance::clean() {
    wgpuInstanceRelease(instance);
    wgpuSwapChainDrop(swapchain);
    wgpuDeviceDrop(device);
    wgpuAdapterDrop(adapter);
}


void WGPUEnv::sInstance::render_frame() {
    // Get the current texture in the swapchain
    {
        current_texture_view = wgpuSwapChainGetCurrentTextureView(swapchain);
        assert_msg(!current_texture_view, "Error, dont resize the window please!!");
    }

    // Create the command encoder
    {
        WGPUCommandEncoderDescriptor encoder_desc = {
            .nextInChain = NULL,
            .label = "Device command encoder"
        };
        device_command_encoder = wgpuDeviceCreateCommandEncoder(device, &encoder_desc);
    }

    // Create & fill the render pass (encoder)
    WGPURenderPassEncoder render_pass;
    {
        // Prepare the color attachment
        WGPURenderPassColorAttachment render_pass_color_attachment = {
            .view = current_texture_view,
            .resolveTarget = NULL, // for MS
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .clearValue = {0.0f,0.0f,1.0f,1.0f}
        };
        WGPURenderPassDescriptor render_pass_descr = {
            .nextInChain = NULL,
            .colorAttachmentCount = 1,
            .colorAttachments = &render_pass_color_attachment,
            .depthStencilAttachment = NULL,
            .timestampWriteCount = 0, // for measuing performance
            .timestampWrites = NULL
        };
        render_pass = wgpuCommandEncoderBeginRenderPass(device_command_encoder, &render_pass_descr);
        // nothing yet
        wgpuRenderPassEncoderEnd(render_pass);
    }
   

    // Submit frame
    {
        wgpuTextureViewDrop(current_texture_view);
        wgpuSwapChainPresent(swapchain);
    }
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

    WGPUDeviceDescriptor descriptor = {
        .nextInChain = NULL,
        .label = "GPU",
        .requiredFeaturesCount = 0,
        .requiredLimits = NULL,
        .defaultQueue = {
            .nextInChain = NULL,
            .label = "Default queue"
        }
    };

    wgpuAdapterRequestDevice(adapter, &descriptor, e_device_request_ended, user_data);
}

void WGPUEnv::sInstance::e_device_request_ended(WGPURequestDeviceStatus status, 
                                                WGPUDevice device, 
                                                char const * message, 
                                                void *user_data) {
    //
    assert_msg(status == WGPURequestDeviceStatus_Success, "Error loading the device");
    ((sPayload*) user_data)->wgpu_man_instance->device = device;

    // Set the error callback
    wgpuDeviceSetUncapturedErrorCallback(device, e_device_error, NULL);
}

void WGPUEnv::sInstance::e_device_error(WGPUErrorType type, char const* message, void* user_data) {
    std::cout << "Error on Device: " << type;
    if (message) {
        std::cout << " - " << message;
    }

    std::cout  << std::endl;
}