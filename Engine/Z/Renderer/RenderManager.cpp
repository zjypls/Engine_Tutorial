//
// Created by z on 24-1-16.
//
#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/RenderManager.h"
#include "Z/Renderer/RenderResource.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"
#include "Z/Utils/ZUtils.h"
#include "Platform/Vulkan/VulkanGraphicInterface.h"

namespace Z {
    Ref<GraphicInterface> RenderManager::m_Context= nullptr;
    Ref<RenderPipeline> RenderManager::renderPipeline=nullptr;
    Ref<ImGuiRendererPlatform> RenderManager::imguiRenderPlatform=nullptr;
    std::vector<std::function<void()>> RenderManager::FuncQueue;
	std::mutex RenderManager::QueueMutex;
    void RenderManager::Init() {
        Z_CORE_ASSERT(m_Context == nullptr,"Already Inited !");
        m_Context= CreateRef<VulkanGraphicInterface>();
        m_Context->Init({});
        AssetsSystem::PreInit();

        RenderResourceInitInfo resourceInfo{};
        resourceInfo.graphicContext=m_Context.get();
        RenderResource::Init(&resourceInfo);
        
        renderPipeline=CreateRef<RenderPipeline>();
        auto info=RenderPipelineInitInfo{};
        info.graphicContext=m_Context.get();
        renderPipeline->Init(&info);
        imguiRenderPlatform=ImGuiRendererPlatform::GetRenderer();
    }

    void RenderManager::DeviceSynchronize() {
        m_Context->DeviceWaiteIdle();
    }

    void RenderManager::Shutdown() {
        imguiRenderPlatform=nullptr;
        renderPipeline->clear();
        RenderResource::clear();
        m_Context->Shutdown();
    }

    void RenderManager::Update(float deltaTime) {

        m_Context->WaitForFences();
        m_Context->ResetCommandPool();
        ExecuteTaskQueue();
        if(m_Context->prepareBeforeRender(RenderManager::Resize))return;

        renderPipeline->draw();
        m_Context->SubmitTask(RenderManager::Resize);
    }

    void RenderManager::PushUIContents(ImGuiContent *content) {
        renderPipeline->PushUIContents(content);
    }

    void RenderManager::SetViewPortSize(uint32 width, uint32 height) {
        m_Context->DeviceWaiteIdle();
        renderPipeline->SetViewPortSize(width,height);
    }

    void *RenderManager::GetViewportFrameBufferDescriptor() {
        return renderPipeline->GetViewportFrameBufferDescriptor();
    }

    
    ImTextureID RenderManager::CreateImGuiTexture(Image* image,ImageView* view,Z::ImageLayout layout){
        ImTextureID id;
        imguiRenderPlatform->CreateTextureForImGui(m_Context->GetDefaultSampler(SamplerType::Linear),view,layout,id);
        return id;
    }

    void RenderManager::Resize() {
        renderPipeline->Resize();
    }

    void RenderManager::ExecuteTaskQueue(){
        if(FuncQueue.empty())return;
        m_Context->DeviceWaiteIdle();
        for(const auto& task:FuncQueue)
            task();
        FuncQueue.clear();
    }

} // Z