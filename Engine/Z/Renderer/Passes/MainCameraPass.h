//
// Created by z on 24-1-31.
//

#ifndef MAINCAMERAPASS_H
#define MAINCAMERAPASS_H
#include "Include/glm/glm.hpp"

#include "Z/Renderer/RenderPass.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
    class Z_API MainCameraPass final : public RenderPass {
        friend class RenderPipeline;
    public:
        static const Z::Format viewportColorFormat = Z::Format::R8G8B8A8_UNORM;
        struct MainCameraPassInitInfo : RenderPassInitInfo {

        };
        enum SubPassIndex {
            _ui_pass_index=0,
            _subpass_count
        };
        enum AttachmentIndex {
            _swapchain_image_index=0,
            _attachment_count
        };
        enum SubpassDependencyIndex {
            _ui_dependency_index=0,
            _dependency_count
        };

        enum viewPortSubPassIndex {
            _viewport_skybox_index=0,
            _viewport_subpass_count
        };
        enum viewPortAttachmentIndex {
            _viewport_color_attachment_index=0,
            _viewport_goindex_attachment_index,
            _viewport_depth_attachment_index,
            _viewport_attachment_count
        };
        enum viewPortSubpassDependencyIndex {
            _viewport_skybox_dependency_index=0,
            _viewport_dependency_count
        };

        void Init(RenderPassInitInfo *info) override;
        void PostInit() override;
        void draw(const Ref<RenderPass>&uiPass);
        void clear() override;
        void SetViewPortSize(uint32 width,uint32 height);
        void* GetViewportFrameBufferDescriptor();
    private:
        void BeginRenderPass();
        void EndRenderPass();
        void InitRenderPass();
        void InitFrameBuffer();
        void InitViewportRenderPass();
        void InitViewportFrameBuffer();

        void DestroyViewportFrameBuffer();

        std::vector<Z::Framebuffer*> swapchainFrameBuffers;
        RenderPassInterface* viewportRenderPass;
        std::vector<RenderPass::Framebuffer> viewportFrameBuffer{};
        std::vector<ImTextureID> viewPortDescriptorSets{};
        glm::uvec2 viewPortSize{800,600};
    };
} // Z

#endif //MAINCAMERAPASS_H
