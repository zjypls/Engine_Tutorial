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
        void WaitForFences(Fence* fence) override;
        void DeviceWaiteIdle() override;
        void ResetCommandPool() override;
        void BeginRenderPass(const RenderPassBeginInfo &info) override{ BeginRenderPass(commandBuffers[currentFrameIndex],info);}
        void BeginRenderPass(CommandBuffer* commandBuffer ,const RenderPassBeginInfo &info) override{ BeginRenderPass(((VulkanCommandBuffer*)commandBuffer)->Get(),info);}
        void EndRenderPass() override;
        void EndRenderPass(CommandBuffer* buffer) override;
        void BindPipeline(PipelineBindPoint,Pipeline *pipeline) override;
        void BindPipeline(CommandBuffer* buffer , PipelineBindPoint bindPoint , Pipeline* pipeline)override;
        void BindDescriptorSets(PipelineBindPoint bindPoint, PipelineLayout *layout, uint32 firstSet, const std::vector<DescriptorSet*>&descriptorSets) override;
        void BindDescriptorSets(CommandBuffer* buffer,PipelineBindPoint bindPoint, PipelineLayout *layout, uint32 firstSet, const std::vector<DescriptorSet*>&descriptorSets) override;
        void BindDescriptorSet(PipelineBindPoint bindPoint,PipelineLayout* layout,DescriptorSet* set)override;
        void PushConstant(PipelineLayout *layout, ShaderStageFlag stage, uint32 offset, uint32 size, const void *data) override;
        void PushConstant(CommandBuffer* buffer,PipelineLayout *layout, ShaderStageFlag stage, uint32 offset, uint32 size, const void *data) override;
        void SetViewPort(const Viewport &viewport) override;
        void SetViewPort(CommandBuffer* buffer , const Viewport& viewport)override;
        void SetScissor(const Rect2D &scissor) override;
        void SetScissor(CommandBuffer* buffer , const Rect2D& scissor)override;
        void BindVertexBuffer(Buffer** buffers,uint32 firstBinding,uint32 bindingCount,uint32 offset) override;
        void BindIndexBuffer(Buffer* buffer,uint32 offset,IndexType indexType) override;
        void BindVertexBuffer(CommandBuffer* commandBuffer,Buffer** buffers,uint32 firstBinding,uint32 bindingCount,uint32 offset) override;
        void BindIndexBuffer(CommandBuffer* commandBuffer,Buffer* buffer,uint32 offset,IndexType indexType) override;
        void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
        void Draw(CommandBuffer* buffer,uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
        void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) override;
        void DrawIndexed(CommandBuffer* commandBuffer,uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) override;

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
                                 GraphicPipelineCreateInfo* createInfo , bool insertInnerSetLayout) override;
        void CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo &info, DescriptorSetLayout *&descriptorSetLayout) override;
        void CreateFence(const FenceCreateInfo& info , Fence * &fence) override;
        void zCreateSemaphore(const SemaphoreCreateInfo& info , Semaphore *& semaphore) override ;
        void AllocateDescriptorSet(const DescriptorSetAllocateInfo &info, DescriptorSet *&descriptorSet) override;
        void AllocateCommandBuffer(const CommandBufferAllocateInfo& info , CommandBuffer*& commandBuffer) override;
        void FreeCommandBuffer(CommandBuffer* buffer) override;
        void BeginCommandBuffer(const CommandBufferBeginInfo& info , CommandBuffer* buffer)override;
        void EndCommandBuffer(CommandBuffer* buffer) override;
        CommandBuffer* BeginOnceCommandSubmit()override;
        void EndOnceSubmit(CommandBuffer* buffer)override;
        void FreeDescriptorSet(DescriptorSet *descriptorSet) override;
        void FreeDescriptorSet(void* descriptorSet)override;
        void WriteDescriptorSets(const WriteDescriptorSet *writes,uint32 writeCount) override;
        void MapMemory(DeviceMemory *memory, uint64 offset, uint64 size, void *&data) override;
        void UnMapMemory(DeviceMemory *memory) override;
        void CopyImageToBuffer(Image* image,Buffer* buffer,ImageLayout layout , const Rect2D& rect ,
                                ImageAspectFlag aspectFlag,
                                uint32 baseLayer ,uint32 layerCount , uint32 baseLevel ) override;
        void PipelineBarrier(PipelineStageFlags srcStageMask,PipelineStageFlags dstStageMask,DependencyFlags dependencyFlags,
                                    uint32 memoryBarrierCount,const zMemoryBarrier* pMemoryBarriers,
                                    uint32_t bufferMemoryBarrierCount,const BufferMemoryBarrier* pBufferMemoryBarriers,
                                    uint32_t imageMemoryBarrierCount,const ImageMemoryBarrier* barrier) override{
            PipelineBarrier(commandBuffers[currentFrameIndex],srcStageMask,dstStageMask,dependencyFlags,memoryBarrierCount,pMemoryBarriers,bufferMemoryBarrierCount,pBufferMemoryBarriers,
                            imageMemoryBarrierCount,barrier);
        }
        void PipelineBarrier(CommandBuffer* buffer,PipelineStageFlags srcStageMask,PipelineStageFlags dstStageMask,DependencyFlags dependencyFlags,
                             uint32 memoryBarrierCount,const zMemoryBarrier* pMemoryBarriers,
                             uint32_t bufferMemoryBarrierCount,const BufferMemoryBarrier* pBufferMemoryBarriers,
                             uint32_t imageMemoryBarrierCount,const ImageMemoryBarrier* barrier) override{
            PipelineBarrier(((VulkanCommandBuffer*)buffer)->Get(),srcStageMask,dstStageMask,dependencyFlags,memoryBarrierCount,pMemoryBarriers,bufferMemoryBarrierCount,pBufferMemoryBarriers,
                            imageMemoryBarrierCount,barrier);
        }
        Sampler* GetDefaultSampler(SamplerType samplerType) override;
        void DestroyRenderPass(RenderPassInterface *renderPassInterface) override;
        std::vector<Z::Framebuffer*> CreateDefaultFrameBuffers(RenderPassInterface *renderPassInterface) override;

        void DestroyFrameBuffer(Z::Framebuffer *framebuffer) override;

        void DestroyImage(Image *image, DeviceMemory *memory, ImageView *view) override;

        void DestroyImageView(ImageView* view)override;

        void DestroyBuffer(Buffer *buffer, DeviceMemory *memory) override;

        void DestroyPipeline(Pipeline *pipeline) override;

        void DestroyPipelineLayout(PipelineLayout *pipelineLayout) override;

        void DestroyDescriptorSetLayout(DescriptorSetLayout *descriptorSetLayout) override;

        void DestroyFence(Fence* fence) override;
        void DestroySemaphore(Semaphore* semaphore) override;


        auto GetInstance(){return instance;}
        auto GetPhysicalDevice(){return physicalDevice;}
        auto GetDevice(){return device;}
        auto GetDescriptorPool(){return descriptorPool;}
        auto GetGraphicQueue(){return graphicsQueue;}
        QueueFamilyIndices& GetQueueFamilyIndices() override{return familyIndices;}
        const SwapChainInfo& GetSwapChainInfo()override{return swapChainInfo;}
        uint32 GetCurrentFrameIndex() override{return currentFrameIndex;}
        uint32 GetMaxFramesInFlight() override {return maxFlightFrames;}

        auto GetCurrentCommandBuffer(){return commandBuffers[currentFrameIndex];}
        std::vector<DescriptorSetLayout*>& GetInnerDescriptorSetLayout()override{return innerSetLayouts;}

        VkCommandBuffer BeginOnceSubmit();
        void EndOnceSubmit(VkCommandBuffer buffer);
    private:

        void PipelineBarrier(VkCommandBuffer buffer,PipelineStageFlags srcStageMask,PipelineStageFlags dstStageMask,DependencyFlags dependencyFlags,
                             uint32 memoryBarrierCount,const zMemoryBarrier* pMemoryBarriers,
                             uint32_t bufferMemoryBarrierCount,const BufferMemoryBarrier* pBufferMemoryBarriers,
                             uint32_t imageMemoryBarrierCount,const ImageMemoryBarrier* barrier) ;
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

        static void BeginRenderPass(VkCommandBuffer buffer,const RenderPassBeginInfo& info);

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
