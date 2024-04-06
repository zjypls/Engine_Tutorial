//
// Created by z on 24-1-18.
//

#ifndef ENGINEALL_VULKANGRACPHICINTERFACE_H
#define ENGINEALL_VULKANGRACPHICINTERFACE_H
#include <vector>
#include "vulkan/vulkan.h"

#include "Z/Renderer/GraphicInterface.h"
#include "Platform/Vulkan/VulkanRenderInterface.h"

namespace Z {
    #ifdef Z_DEBUG
        constexpr bool enableVulkanDebug=true;
    #else
        constexpr bool enableVulkanDebug=false;
    #endif
    constexpr uint32 maxVertexBlendCount=128,maxMaterialCount=128;
    constexpr VkSampleCountFlagBits defaultSampleFlag=VK_SAMPLE_COUNT_1_BIT;

    class Z_API VulkanGraphicInterface final : public GraphicInterface{
    public:
        //Render Interface
        void Init(const GraphicSpec&spec)override;
        void Shutdown() override;
        bool prepareBeforeRender(const std::function<void()>&funcCallAfterRecreateSwapChain) override;
        void SubmitTask(const std::function<void()>&funcCallAfterRecreateSwapChain) override;
        void WaitForFences() override;
        void DeviceWaiteIdle() override;
        void ResetCommandPool() override;
        void BeginRenderPass(const RenderPassBeginInfo &info) override;
        void EndRenderPass() override;
        void BindPipeline(PipelineBindPoint,Pipeline *pipeline) override;
        void BindDescriptorSets(PipelineBindPoint bindPoint, PipelineLayout *layout, uint32 firstSet, const std::vector<DescriptorSet*>&descriptorSets) override;
        void BindDescriptorSet(PipelineBindPoint bindPoint,PipelineLayout* layout,DescriptorSet* set)override;
        void PushConstant(PipelineLayout *layout, ShaderStageFlag stage, uint32 offset, uint32 size, const void *data) override;
        void SetViewPort(const Viewport &viewport) override;
        void SetScissor(const Rect2D &scissor) override;
        void BindVertexBuffer(Buffer** buffers,uint32 firstBinding,uint32 bindingCount,uint32 offset) override;
        void BindIndexBuffer(Buffer* buffer,uint32 offset,IndexType indexType) override;
        void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
        void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) override;

        //Resource create interface
        void CreateImage(const ImageInfo& info,Image*& image,DeviceMemory*& memory)override;
        void CreateImage(const ImageInfo& info,Image*& image,DeviceMemory*& memory,ImageView*& imageView,void* pixelData)override;
        void CreateCubeMap(const ImageInfo& info,Image*& image,DeviceMemory*& memory,ImageView*& imageView,const std::array<void*,6>& pixelData)override;
        void CreateFrameBuffer(const FramebufferInfo&info,Z::Framebuffer*&frameBuffer)override;
        void CreateImageView(const ImageViewInfo& info,ImageView*& imageView)override;
        void CreateBuffer(const BufferInfo& info,Buffer*&buffer,DeviceMemory*& memory)override;
        void CreateBuffer(const BufferInfo& info,Buffer*& buffer,DeviceMemory*& memory,void* data)override;
        void CreateShaderModule(const ShaderModuleCreateInfo &moduleInfo, ShaderModule *&module) override;
        void CreateRenderPass(const RenderPassCreateInfo &info, RenderPassInterface *&renderPassInterface) override;
        void CreatePipelineLayout(const PipelineLayoutCreateInfo& info,PipelineLayout*& layout) override;
        void CreateGraphicPipeline(const GraphicPipelineCreateInfo &createInfo, Pipeline *&graphicPipeline) override;
        std::vector<DescriptorSetInfo> CreateGraphicPipeline(const std::string &shaderSources, const std::vector<Z::ShaderStageFlag> &stageFlags,
                                 Pipeline *&graphicPipeline, RenderPassInterface *renderPassInterface,
                                 std::vector<DescriptorSetLayout*>&descriptorSetLayout,PipelineLayout*&pipelineLayout,
                                 GraphicPipelineCreateInfo* createInfo) override;
        void CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo &info, DescriptorSetLayout *&descriptorSetLayout) override;
        void AllocateDescriptorSet(const DescriptorSetAllocateInfo &info, DescriptorSet *&descriptorSet) override;
        void FreeDescriptorSet(DescriptorSet *descriptorSet) override;
        void FreeDescriptorSet(void* descriptorSet)override;
        void WriteDescriptorSets(const WriteDescriptorSet *writes,uint32 writeCount) override;
        void MapMemory(DeviceMemory *memory, uint64 offset, uint64 size, void *&data) override;
        void UnMapMemory(DeviceMemory *memory) override;
        Sampler* GetDefaultSampler(SamplerType samplerType) override;
        void DestroyRenderPass(RenderPassInterface *renderPassInterface) override;
        std::vector<Z::Framebuffer*> CreateDefaultFrameBuffers(RenderPassInterface *renderPassInterface) override;

        void DestroyFrameBuffer(Z::Framebuffer *framebuffer) override;

        void DestroyImage(Image *image, DeviceMemory *memory, ImageView *view) override;

        void DestroyBuffer(Buffer *buffer, DeviceMemory *memory) override;

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
        std::vector<DescriptorSetLayout*>& GetInnerDescriptorSetLayout()override{return innerSetLayouts;}

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

        void DestroySwapchain();
        void ReCreateSwapChain();

        void InitInnerSetLayout();

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

        bool enableValidationLayer=enableVulkanDebug,enableDebugUtils=enableVulkanDebug;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFrameBuffers;
        SwapChainInfo swapChainInfo;
        VkFormat swapchainFormat;
        VkExtent2D swapchainExtent;
        VkRect2D scissor;

        Sampler *defaultLinearSampler,*defaultNearestSampler;
        std::vector<DescriptorSetLayout*> innerSetLayouts;

        const std::vector<const char *> validationLayers {"VK_LAYER_KHRONOS_validation"};
    };
} // Z

#endif //ENGINEALL_VULKANGRACPHICINTERFACE_H
