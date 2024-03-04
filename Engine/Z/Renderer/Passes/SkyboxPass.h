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
    };
    class Z_API SkyboxPass : public RenderPass {
    public:
        void Init(RenderPassInitInfo*info) override;
        void draw() override;
        void clear() override;
    protected:
    };

} // Z

#endif //ENGINEALL_SKYBOXPASS_H
