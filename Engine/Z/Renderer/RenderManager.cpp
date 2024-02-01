//
// Created by z on 24-1-16.
//
#include "Z/Core/Log.h"

#include "Z/Renderer/RenderManager.h"

#include "Platform/Vulkan/VulkanGraphicInterface.h"

namespace Z {
    Ref<GraphicInterface> RenderManager::m_Context= nullptr;
    Ref<RenderPipeline> RenderManager::renderPipeline=nullptr;
    void RenderManager::Init() {
        Z_CORE_ASSERT(m_Context == nullptr,"Already Inited !");
        m_Context= CreateRef<VulkanGraphicInterface>();
        m_Context->Init({});

        renderPipeline=CreateRef<RenderPipeline>();
        auto info=RenderPipelineInitInfo{};
        info.graphicContext=m_Context.get();
        renderPipeline->Init(&info);
    }

    void RenderManager::DeviceSynchronize() {
        m_Context->DeviceWaiteIdle();
    }

    void RenderManager::Shutdown() {
        m_Context->Shutdown();
    }

    void RenderManager::Update(float deltaTime) {

        m_Context->WaitForFences();

        m_Context->ResetCommandPool();
        m_Context->prepareBeforeRender({});
        renderPipeline->draw();
        m_Context->SubmitTask();
    }

    void RenderManager::InitUIRenderBackend() {
        renderPipeline->InitUIRenderBackend();
    }

    void RenderManager::PushUIContents(ImGuiContent *content) {
        renderPipeline->PushUIContents(content);
    }

} // Z