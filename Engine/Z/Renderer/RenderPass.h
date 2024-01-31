//
// Created by z on 24-1-28.
//

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vector>

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderResource.h"

namespace Z {
    struct RenderPassInitInfo {
        GraphicInterface* graphicInterface;
    };
    class Z_API RenderPass {
    public:
        struct FrameBufferAttachment
        {
            Image*        image;
            DeviceMemory* memory;
            ImageView*    view;
            Format       format;
        };

        struct Framebuffer
        {
            int           width;
            int           height;
            Z::Framebuffer* framebuffer;
            RenderPassInterface*  renderPass;

            std::vector<FrameBufferAttachment> attachments;
        };

        struct Descriptor
        {
            DescriptorSetLayout* layout;
            DescriptorSet*       descriptorSet;
        };

        struct RenderPipelineBase
        {
            PipelineLayout* layout;
            Pipeline*       pipeline;
        };

        virtual void Init(RenderPassInitInfo*info)=0;
        virtual void draw(){};

    protected:
        [[nodiscard]]
        RenderPassInterface* GetRenderPass()const{return framebuffer.renderPass;}
        GraphicInterface* Context=nullptr;
        std::vector<Descriptor>         descriptors{};
        std::vector<RenderPipelineBase> renderPipelines{};
        Framebuffer                     framebuffer{};

    };

} // Z

#endif //RENDERPASS_H
