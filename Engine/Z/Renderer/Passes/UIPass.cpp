//
// Created by z on 24-1-29.
//

#include "Include/imgui/backends/imgui_impl_glfw.h"

#include "Z/Renderer/Passes/UIPass.h"

#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
    void UIPass::Init(RenderPassInitInfo*info) {
        framebuffer.render_pass=((UIPassInitInfo*)info)->renderpass;
    }

    void UIPass::draw() {
        ImGuiRendererPlatform::GetRenderer()->Begin();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        for(const auto content:ui_Contents) {
            content->OnImGuiRender();
        }
        ImGui::Render();
        ImGuiRendererPlatform::GetRenderer()->End();
    }
} // Z