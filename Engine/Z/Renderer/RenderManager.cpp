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
    void RenderManager::Init() {
        Z_CORE_ASSERT(m_Context == nullptr,"Already Inited !");
        m_Context= CreateRef<VulkanGraphicInterface>();
        m_Context->Init({});
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
        m_Context->prepareBeforeRender({});
        renderPipeline->draw();
        m_Context->SubmitTask();
    }

    void RenderManager::PushUIContents(ImGuiContent *content) {
        renderPipeline->PushUIContents(content);
    }

    ImTextureID RenderManager::CreateImGuiTexture(Image* image,ImageView* view,Z::ImageLayout layout){
        ImTextureID id;
        imguiRenderPlatform->CreateTextureForImGui(m_Context->GetDefaultSampler(SamplerType::Linear),view,layout,id);
        return id;
    }

} // Z