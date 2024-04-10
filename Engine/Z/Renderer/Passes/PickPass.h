//
// Created by z on 2024/4/9.
//

#ifndef ENGINEALL_PICKPASS_H
#define ENGINEALL_PICKPASS_H

#include "Z/Renderer/RenderPass.h"

namespace Z {

    struct PickPassInitInfo : public RenderPassInitInfo{

    };

    class PickPass  final : public RenderPass  {
    public:
        void Init(RenderPassInitInfo* info)override;

        void draw()override;

        void clear()override;

        void SetViewportSize(uint32 width,uint32 height);

        int GetPickValue(int x,int y);

    private:
        void InitRenderPass();
        void InitFrameBuffer();
        void InitPipeline();

        void clearFramebuffer();

        glm::uvec2 viewPortSize{800,600};

        std::vector<DescriptorSetLayout*> noVertexBlendingLay;
    };

} // Z

#endif //ENGINEALL_PICKPASS_H
