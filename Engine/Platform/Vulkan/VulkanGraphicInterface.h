//
// Created by z on 24-1-18.
//

#ifndef ENGINEALL_VULKANGRACPHICINTERFACE_H
#define ENGINEALL_VULKANGRACPHICINTERFACE_H
#include <vector>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "Z/Renderer/GraphicInterface.h"
#include "Platform/Vulkan/VulkanResource.h"

namespace Z {

    constexpr uint32 maxFlightFrames=2;
    constexpr uint32 maxVertexBlendCount=128,maxMaterialCount=128;

    class VulkanGraphicInterface final : public GraphicInterface{
    public:
        void CreateImage(const ImageInfo& info,Image*& image,DeviceMemory*& memory)override;
        void CreateBuffer(const BufferInfo& info,Buffer*&buffer,DeviceMemory*& memory)override;
        void Init(const GraphicSpec&spec)override;
        auto GetInstance(){return instance;}
        auto GetPhysicalDevice(){return physicalDevice;}
        auto GetDevice(){return device;}
        auto GetDescriptorPool(){return descriptorPool;}
        auto GetGraphicQueue(){return graphicsQueue;}
        auto GetQueueFamily(){return familyIndices;}
    private:


        void CreateInstance();
        void initializeDebugMessenger();
        void CreateWindowSurface();
        void initializePhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateDescriptorPool();
        void CreateSyncSignals();
        void CreateSwapchain();
        void CreateSwapchainImageViews();
        void CreateFramebufferImageAndView();
        void CreateVmaAllocator();

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugUtilsMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSwapchainKHR swapchain;
        QueueFamilyIndices familyIndices;
        VkFormat depthFormat;
        Queue* graphicsQueue,*computeQueue;
        VkCommandPool transientCommandPool;
        VkCommandPool commandPools[maxFlightFrames];
        VkCommandBuffer commandBuffers[maxFlightFrames];
        VkDescriptorPool descriptorPool;
        VmaAllocator vmaAllocator;
        VkFence frameFences[maxFlightFrames];
        VkSemaphore imageAvailable[maxFlightFrames],imageRenderFinish[maxFlightFrames];

        bool enableValidationLayer=true,enableDebugUtils=true;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkFormat swapchainFormat;
        VkExtent2D swapchainExtent;
        VkRect2D scissor;

        const std::vector<char const*> validationLayers {"VK_LAYER_KHRONOS_validation"};
    };

} // Z

#endif //ENGINEALL_VULKANGRACPHICINTERFACE_H
