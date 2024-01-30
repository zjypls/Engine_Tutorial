//
// Created by z on 24-1-28.
//

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vector>

#include "Z/Renderer/RenderResource.h"

namespace Z {
    struct RenderPassInitInfo {

    };
    class RenderPass {
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
            Framebuffer* framebuffer;
            RenderPassInterface*  render_pass;

            std::vector<FrameBufferAttachment> attachments;
        };

        struct Descriptor
        {
            DescriptorSetLayout* layout;
            DescriptorSet*       descriptor_set;
        };

        struct RenderPipelineBase
        {
            PipelineLayout* layout;
            Pipeline*       pipeline;
        };

        virtual void Init(RenderPassInitInfo*info)=0;
        virtual void draw()=0;

    protected:
        std::vector<Descriptor>         descriptors{};
        std::vector<RenderPipelineBase> renderPipelines{};
        Framebuffer                     framebuffer{};

    };

} // Z

#endif //RENDERPASS_H
