//
// Created by z on 24-2-24.
//

#ifndef ENGINEALL_SKYBOXPASS_H
#define ENGINEALL_SKYBOXPASS_H

#include "Z/Core/Core.h"
#include "Z/Renderer/RenderPass.h"

namespace Z {
    struct SkyboxPassInitInfo : RenderPassInitInfo {
        RenderPassInterface* renderpass;
        RenderPass::Framebuffer *framebuffer;
        Buffer *mvpMatrixBuffer;
        uint32 width;
        uint32 height;
        uint32 frameBufferCount;
    };
    class Z_API SkyboxPass : public RenderPass {
    public:
        void Init(RenderPassInitInfo*info) override;
        void draw() override;
        void clear() override;
        void SetViewportSize(uint32 width,uint32 height){framebuffer.width=width;framebuffer.height=height;}
    protected:
        void InitPipelineLayout();
        void InitPipeline();
        void InitDescriptorSets(SkyboxPassInitInfo* initInfo);
        RenderPass::Framebuffer* viewportFramebuffer;
        uint32 viewportFrameBufferCount;
        ImageView* skyboxView= nullptr;
    };

} // Z

#endif //ENGINEALL_SKYBOXPASS_H
