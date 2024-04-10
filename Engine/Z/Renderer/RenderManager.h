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
        // label for default set bind
        enum InnerDescriptorSet {
            CameraDataTransformSet=0,
            WorldLightDataSet,
            BoneNodeTransformSet,
            MaterialTextureSet
        };
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

        static glm::uvec2 GetViewportSize(){return renderPipeline->GetViewportSize();}

        static void PickGO(int  x, int y,const std::function<void(int)>& func){
            SubmitResourceUpdateTask([x,y,func]{
                func(renderPipeline->PickGO(x,y));
            });
        }

        static Ref<RenderPass> GetPass(const std::string& path){
            const auto& generalPassMap=renderPipeline->generalPassMap;
            auto passIt=generalPassMap.find(path);
            if(passIt!=generalPassMap.end())return passIt->second;
            return renderPipeline->LoadPass(path);
        }
        // submit a task for update graphic resources after gpu render task finished
        // to avoid update resources when it is using that cause vulkan error
        static void SubmitResourceUpdateTask(const std::function<void()>&func){
            std::scoped_lock<std::mutex> lock(QueueMutex);
            FuncQueue.push_back(func);
        }

    private:

        static void ExecuteTaskQueue();

        static Ref<GraphicInterface> m_Context;
        static Ref<RenderPipeline> renderPipeline;
        static Ref<ImGuiRendererPlatform> imguiRenderPlatform;
        static std::vector<std::function<void()>> FuncQueue;
		static std::mutex QueueMutex;
    };

} // Z

#endif //ENGINEALL_RENDERMANAGER_H
