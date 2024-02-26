//
// Created by z on 24-1-18.
//

#ifndef ENGINEALL_VULKANGRACPHICINTERFACE_H
#define ENGINEALL_VULKANGRACPHICINTERFACE_H
#include <vector>
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include "Z/Renderer/GraphicInterface.h"
#include "Platform/Vulkan/VulkanRenderInterface.h"

namespace Z {
    constexpr uint32 maxVertexBlendCount=128,maxMaterialCount=128;
    constexpr VkSampleCountFlagBits defaultSampleFlag=VK_SAMPLE_COUNT_1_BIT;

    class Z_API VulkanGraphicInterface final : public GraphicInterface{
    public:
        //Render Interface
        void Init(const GraphicSpec&spec)override;
        void Shutdown() override;
        bool prepareBeforeRender(const std::function<void()>&funcCallAfterRecreateSwapChain) override;
        void SubmitTask() override;
        void WaitForFences() override;
        void DeviceWaiteIdle() override;
        void ResetCommandPool() override;
        void BeginRenderPass(const RenderPassBeginInfo &info) override;
        void EndRenderPass() override;

        //Resource create interface
        void CreateImage(const ImageInfo& info,Image*& image,DeviceMemory*& memory)override;
        void CreateFrameBuffer(const FramebufferInfo&info,Z::Framebuffer*&frameBuffer)override;
        void CreateImageView(const ImageViewInfo& info,ImageView*& imageView)override;
        void CreateBuffer(const BufferInfo& info,Buffer*&buffer,DeviceMemory*& memory)override;
        void CreateShaderModule(const ShaderModuleCreateInfo &moduleInfo, ShaderModule *&module) override;
        void CreateRenderPass(const RenderPassCreateInfo &info, RenderPassInterface *&renderPassInterface) override;
        void CreateGraphicPipeline(const GraphicPipelineCreateInfo &createInfo, Pipeline *&graphicPipeline) override;
        void CreateGraphicPipeline(const std::string &shaderSources, const std::vector<Z::ShaderStageFlag> &stageFlags,
                                 Pipeline *&graphicPipeline, RenderPassInterface *renderPassInterface,
                                 DescriptorSetLayout*&descriptorSetLayout,PipelineLayout*&pipelineLayout,
                                 GraphicPipelineCreateInfo* createInfo) override;
        void DestroyRenderPass(RenderPassInterface *renderPassInterface) override;
        std::vector<Z::Framebuffer*> CreateDefaultFrameBuffers(RenderPassInterface *renderPassInterface) override;

        void DestroyFrameBuffer(Z::Framebuffer *framebuffer) override;

        void DestroyImage(Image *image, DeviceMemory *memory, ImageView *view) override;

        void DestroyPipeline(Pipeline *pipeline) override;

        void DestroyPipelineLayout(PipelineLayout *pipelineLayout) override;

        void DestroyDescriptorSetLayout(DescriptorSetLayout *descriptorSetLayout) override;


        auto GetInstance(){return instance;}
        auto GetPhysicalDevice(){return physicalDevice;}
        auto GetDevice(){return device;}
        auto GetDescriptorPool(){return descriptorPool;}
        auto GetGraphicQueue(){return graphicsQueue;}
        auto GetQueueFamily(){return familyIndices;}
        const SwapChainInfo& GetSwapChainInfo()override{return swapChainInfo;}
        uint32 GetCurrentFrameIndex() override{return currentFrameIndex;}
        uint32 GetMaxFramesInFlight() override {return maxFlightFrames;}

        auto GetCurrentCommandBuffer(){return commandBuffers[currentFrameIndex];}

        VkCommandBuffer BeginOnceSubmit();
        void EndOnceSubmit(VkCommandBuffer buffer);
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
        void CreateDefaultSampler();

        void ReCreateSwapChain();

        uint32 maxFlightFrames=0;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugUtilsMessenger;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSwapchainKHR swapchain;
        uint32 currentSwapChainImageIndex=0;
        QueueFamilyIndices familyIndices;
        VkFormat depthFormat;
        Queue* graphicsQueue,*computeQueue,*presentQueue;
        VkCommandPool transientCommandPool;
        std::vector<VkCommandPool> commandPools;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFence> frameFences;
        std::vector<VkSemaphore> imageAvailable,imageRenderFinish;
        uint32 currentFrameIndex=0;
        VkDescriptorPool descriptorPool;
        VmaAllocator vmaAllocator;

        bool enableValidationLayer=true,enableDebugUtils=true;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFrameBuffers;
        SwapChainInfo swapChainInfo;
        VkFormat swapchainFormat;
        VkExtent2D swapchainExtent;
        VkRect2D scissor;

        Sampler *defaultLinearSampler,*defaultNearestSampler;

        const std::vector<const char *> validationLayers {"VK_LAYER_KHRONOS_validation"};
    };
} // Z

#endif //ENGINEALL_VULKANGRACPHICINTERFACE_H
