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

        virtual void SubmitTask()=0;

        virtual void WaitForFences()=0;

        virtual void DeviceWaiteIdle()=0;

        virtual void ResetCommandPool()=0;

        virtual void CreateImage(const ImageInfo &info, Image* &image, DeviceMemory* &memory) = 0;

        virtual void CreateFrameBuffer(const FramebufferInfo&info,Z::Framebuffer*&frameBuffer)=0;

        virtual void CreateImageView(const ImageViewInfo &info, ImageView* &imageView) = 0;

        virtual void CreateBuffer(const BufferInfo &info, Buffer* &buffer, DeviceMemory* &memory) = 0;

        virtual void CreateShaderModule(const ShaderModuleCreateInfo& moduleInfo,ShaderModule*& module)=0;

        virtual void CreateRenderPass(const RenderPassCreateInfo& info,RenderPassInterface*&renderPassInterface)=0;

        virtual void CreateGraphicPipeline(const GraphicPipelineCreateInfo& createInfo, Pipeline*&graphicPipeline) = 0;

        virtual void CreateGraphicPipeline(const std::string&shaderSources,const std::vector<Z::ShaderStageFlag>&stageFlags,Pipeline*&graphicPipeline,
                        RenderPassInterface* renderPassInterface,std::vector<DescriptorSetLayout*>&descriptorSetLayout,
                        PipelineLayout*&pipelineLayout , GraphicPipelineCreateInfo* createInfo=nullptr) = 0;

        virtual void DestroyRenderPass(RenderPassInterface* renderPassInterface)=0;

        virtual std::vector<Z::Framebuffer*> CreateDefaultFrameBuffers(RenderPassInterface* renderPassInterface)=0;

        virtual void DestroyFrameBuffer(Z::Framebuffer* framebuffer) = 0;

        virtual void DestroyImage(Image* image, DeviceMemory* memory , ImageView* view = nullptr) = 0;

        virtual void DestroyPipeline(Pipeline* pipeline)=0;

        virtual void DestroyPipelineLayout(PipelineLayout* pipelineLayout)=0;

        virtual void DestroyDescriptorSetLayout(DescriptorSetLayout* descriptorSetLayout)=0;

        virtual void BeginRenderPass(const RenderPassBeginInfo&info)=0;

        virtual void EndRenderPass()=0;

        virtual const SwapChainInfo& GetSwapChainInfo()=0;

        virtual uint32 GetCurrentFrameIndex()=0;

        virtual uint32 GetMaxFramesInFlight()=0;
    };

} // Z

#endif //ENGINEALL_GRAPHICINTERFACE_H
