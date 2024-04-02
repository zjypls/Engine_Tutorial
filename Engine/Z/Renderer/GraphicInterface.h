//
// Created by z on 24-1-15.
//

#ifndef ENGINEALL_GRAPHICINTERFACE_H
#define ENGINEALL_GRAPHICINTERFACE_H

#include <functional>

#include "Z/Renderer/RenderInterface.h"

namespace Z {

    struct GraphicSpec {

    };

    class Z_API GraphicInterface {
    public:
        virtual void Init(const GraphicSpec &initInfo) = 0;

        virtual void Shutdown() = 0;

        virtual bool prepareBeforeRender(const std::function<void()>&funcCallAfterRecreateSwapChain)=0;

        virtual void SubmitTask(const std::function<void()>&funcCallAfterRecreateSwapChain)=0;

        virtual void WaitForFences()=0;

        virtual void DeviceWaiteIdle()=0;

        virtual void ResetCommandPool()=0;

        virtual void CreateImage(const ImageInfo &info, Image* &image, DeviceMemory* &memory) = 0;

        virtual void CreateImage(const ImageInfo &info, Image* &image, DeviceMemory* &memory, ImageView*& imageView , void* pixelData) = 0;

        virtual void CreateCubeMap(const ImageInfo &info, Image* &image, DeviceMemory* &memory , ImageView*& imageView,const std::array<void*,6>& pixelData) = 0;

        virtual void CreateFrameBuffer(const FramebufferInfo&info,Z::Framebuffer*&frameBuffer)=0;

        virtual void CreateImageView(const ImageViewInfo &info, ImageView* &imageView) = 0;

        virtual void CreateBuffer(const BufferInfo &info, Buffer* &buffer, DeviceMemory* &memory) = 0;

        virtual void CreateBuffer(const BufferInfo& info,Buffer* &buffer, DeviceMemory* &memory, void* data) = 0;

        virtual void CreateShaderModule(const ShaderModuleCreateInfo& moduleInfo,ShaderModule*& module)=0;

        virtual void CreateRenderPass(const RenderPassCreateInfo& info,RenderPassInterface*&renderPassInterface)=0;

        virtual void CreateGraphicPipeline(const GraphicPipelineCreateInfo& createInfo, Pipeline*&graphicPipeline) = 0;

        virtual std::vector<DescriptorSetInfo> CreateGraphicPipeline(const std::string&shaderSources,const std::vector<Z::ShaderStageFlag>&stageFlags,Pipeline*&graphicPipeline,
                        RenderPassInterface* renderPassInterface,std::vector<DescriptorSetLayout*>&descriptorSetLayout,
                        PipelineLayout*&pipelineLayout , GraphicPipelineCreateInfo* createInfo=nullptr) = 0;

        virtual void CreateDescriptorSetLayout(const DescriptorSetLayoutCreateInfo& info, DescriptorSetLayout* &descriptorSetLayout) = 0;

        //make info.DescriptorPool empty  to use default pool
        virtual void AllocateDescriptorSet(const DescriptorSetAllocateInfo& info, DescriptorSet* &descriptorSet) = 0;

        virtual void FreeDescriptorSet(DescriptorSet* descriptorSet) = 0;

        virtual void FreeDescriptorSet(void* descriptorSet) = 0;

        virtual void WriteDescriptorSets(const WriteDescriptorSet* writes,uint32 writeCount) = 0;

        virtual Sampler* GetDefaultSampler(SamplerType samplerType) = 0;

        virtual void DestroyRenderPass(RenderPassInterface* renderPassInterface)=0;

        virtual std::vector<Z::Framebuffer*> CreateDefaultFrameBuffers(RenderPassInterface* renderPassInterface)=0;

        virtual void DestroyFrameBuffer(Z::Framebuffer* framebuffer) = 0;

        virtual void DestroyImage(Image* image, DeviceMemory* memory , ImageView* view = nullptr) = 0;

        virtual void DestroyBuffer(Buffer* buffer, DeviceMemory* memory) = 0;

        virtual void DestroyPipeline(Pipeline* pipeline)=0;

        virtual void DestroyPipelineLayout(PipelineLayout* pipelineLayout)=0;

        virtual void DestroyDescriptorSetLayout(DescriptorSetLayout* descriptorSetLayout)=0;

        virtual void BeginRenderPass(const RenderPassBeginInfo&info)=0;

        virtual void EndRenderPass()=0;

        virtual void BindPipeline(PipelineBindPoint bindPoint,Pipeline* pipeline)=0;

        virtual void SetViewPort(const Viewport& viewPort)=0;

        virtual void SetScissor(const Rect2D& scissor)=0;

        virtual void BindDescriptorSets(PipelineBindPoint bindPoint,PipelineLayout* layout,uint32 firstSet,const std::vector<DescriptorSet*>& descriptorSets)=0;

        virtual void BindDescriptorSet(PipelineBindPoint bindPoint,PipelineLayout* layout,DescriptorSet* set) = 0;

        virtual void PushConstant(PipelineLayout* layout,ShaderStageFlag stageFlags,uint32 offset,uint32 size,const void* data)=0;

        virtual void BindVertexBuffer(Buffer** buffer,uint32 firstBinding,uint32 bindingCount,uint32 offset=0)=0;

        virtual void BindIndexBuffer(Buffer* buffer,uint32 offset=0, IndexType indexType = IndexType::UINT32)=0;

        virtual void Draw(uint32 vertexCount,uint32 instanceCount,uint32 firstVertex,uint32 firstInstance)=0;

        virtual void DrawIndexed(uint32 indexCount,uint32 instanceCount,uint32 firstIndex,uint32 vertexOffset,uint32 firstInstance)=0;

        virtual void MapMemory(DeviceMemory* memory, uint64 offset, uint64 size, void*& data)=0;

        virtual void UnMapMemory(DeviceMemory* memory)=0;

        virtual const SwapChainInfo& GetSwapChainInfo()=0;

        virtual uint32 GetCurrentFrameIndex()=0;

        virtual uint32 GetMaxFramesInFlight()=0;

        virtual DescriptorSetLayout* GetFirstDescriptorSetLayout()=0;
    };

} // Z

#endif //ENGINEALL_GRAPHICINTERFACE_H
