//
// Created by z on 24-1-30.
//

#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "Z/Core/Core.h"
#include "Z/Core/AssetsSystem.h"
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

        virtual void PushUIContents(ImGuiContent* content);

        virtual void clear();

        void SetViewPortSize(uint32 width,uint32 height);

        void* GetViewportFrameBufferDescriptor();

        void Resize();

        glm::uvec2 GetViewportSize();

        Ref<RenderPass> LoadPass(const std::string& path);

        int PickGO(int x, int y);

        Ref<RenderPass> genShader;
    protected:
        GraphicInterface* Context;
        Ref<RenderPass> mainCameraPass;
        Ref<RenderPass> uiPass;
        Ref<RenderPass> skyboxPass;
        Ref<RenderPass> convertTool;
        Ref<RenderPass> pickPass;
        std::unordered_map<std::string,Ref<RenderPass>> generalPassMap;
        Texture2D* skybox;
    };

} // Z

#endif //RENDERPIPELINE_H
