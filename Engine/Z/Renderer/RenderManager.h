//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_RENDERMANAGER_H
#define ENGINEALL_RENDERMANAGER_H

#include "Z/ImGui/ImGuiContent.h"

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderPipeline.h"
namespace Z {

    class Z_API RenderManager final{
    public:
        static void Init();
        static void DeviceSynchronize();
        static void Shutdown();
        static auto GetInstance(){return m_Context;}

        static void Update(float deltaTime);
        static void InitUIRenderBackend();
        static void PushUIContents(ImGuiContent* content);

    private:

        static Ref<GraphicInterface> m_Context;
        static Ref<RenderPipeline> renderPipeline;
    };

} // Z

#endif //ENGINEALL_RENDERMANAGER_H
