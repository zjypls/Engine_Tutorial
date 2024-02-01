//
// Created by z on 24-1-15.
//

#ifndef ENGINEALL_GRAPHICINTERFACE_H
#define ENGINEALL_GRAPHICINTERFACE_H

#include <functional>

#include "Z/Renderer/RenderResource.h"

namespace Z {

    struct GraphicSpec {

    };

    class Z_API GraphicInterface {
    public:
        virtual void Init(const GraphicSpec &initInfo) = 0;

        virtual bool prepareBeforeRender(const std::function<void()>&funcCallAfterRecreateSwapChain)=0;

        virtual void SubmitTask()=0;

        virtual void WaitForFences()=0;

        virtual void DeviceWaiteIdle()=0;

        virtual void ResetCommandPool()=0;

        virtual void CreateImage(const ImageInfo &info, Image* &image, DeviceMemory* &memory) = 0;

        virtual void CreateBuffer(const BufferInfo &info, Buffer* &buffer, DeviceMemory* &memory) = 0;

        virtual void CreateRenderPass(const RenderPassCreateInfo& info,RenderPassInterface*&renderPassInterface)=0;

        virtual void BeginRenderPass(const RenderPassBeginInfo&info)=0;

        virtual void EndRenderPass()=0;

        virtual const SwapChainInfo& GetSwapChainInfo()=0;

        virtual uint32 GetCurrentFrameIndex()=0;
    };

} // Z

#endif //ENGINEALL_GRAPHICINTERFACE_H
