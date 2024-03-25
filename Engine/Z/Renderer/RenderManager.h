//
// Created by z on 24-1-16.
//

#ifndef ENGINEALL_RENDERMANAGER_H
#define ENGINEALL_RENDERMANAGER_H

#include "Z/ImGui/ImGuiContent.h"

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderPipeline.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"
#include "Z/Core/AssetsSystem.h"

namespace Z {

    class Z_API RenderManager final{
    public:
        static void Init();
        static void DeviceSynchronize();
        static void Shutdown();
        static auto GetInstance(){return m_Context;}

        static void Update(float deltaTime);
        static void PushUIContents(ImGuiContent* content);

        static void* GetViewportFrameBufferDescriptor();

        static void SetViewPortSize(uint32 width,uint32 height);

        static ImTextureID CreateImGuiTexture(Image* image,ImageView* view,Z::ImageLayout layout=Z::ImageLayout::SHADER_READ_ONLY_OPTIMAL);
        
        static ImTextureID CreateImGuiTexture(Texture2D* texture2D){return CreateImGuiTexture(texture2D->image,texture2D->imageView);}

        static void Resize();

    private:
        static Ref<GraphicInterface> m_Context;
        static Ref<RenderPipeline> renderPipeline;
        static Ref<ImGuiRendererPlatform> imguiRenderPlatform;
    };

} // Z

#endif //ENGINEALL_RENDERMANAGER_H
