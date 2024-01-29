//
// Created by z on 24-1-16.
//
#include "Z/Core/Log.h"

#include "Z/Renderer/RenderManager.h"

#include "Platform/Vulkan/VulkanGraphicInterface.h"

namespace Z {
    Ref<GraphicInterface> RenderManager::m_Context= nullptr;
    void RenderManager::Init() {
        Z_CORE_ASSERT(m_Context == nullptr,"Already Inited !");
        m_Context= CreateRef<VulkanGraphicInterface>();
        m_Context->Init({});
    }

    void RenderManager::Update(float deltaTime) {
    }
} // Z