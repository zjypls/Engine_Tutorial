//
// Created by z on 24-1-30.
//

#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "Z/Core/Core.h"
#include "Z/ImGui/ImGuiContent.h"

#include "Z/Renderer/RenderPass.h"

namespace Z {

    class GraphicInterface;

    struct RenderPipelineInitInfo {
        GraphicInterface* graphicContext;
    };
    class Z_API RenderPipeline {
        friend class RenderManager;
    public:

        virtual void Init(RenderPipelineInitInfo*info);

        virtual void draw();

        virtual void InitUIRenderBackend();

        virtual void PushUIContents(ImGuiContent* content);

    protected:
        GraphicInterface* Context;
        Ref<RenderPass> mainCameraPass;
        Ref<RenderPass> uiPass;
    };

} // Z

#endif //RENDERPIPELINE_H