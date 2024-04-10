//
// Created by z on 2024/3/31.
//

#ifndef ENGINEALL_GENERALPASS_H
#define ENGINEALL_GENERALPASS_H

#include "Z/Renderer/RenderPass.h"

namespace Z {

    struct GeneralPassInitInfo : public RenderPassInitInfo{
        RenderPassInterface* renderpass;
        std::string shaderPath;
    };

    class Z_API GeneralPass : public RenderPass{
    public:
        void Init(RenderPassInitInfo* initInfo)override;

        void draw()override;

        void clear()override;


    private:

        void SetupPipeline();

        std::string materialPath;
    };

} // Z

#endif //ENGINEALL_GENERALPASS_H
