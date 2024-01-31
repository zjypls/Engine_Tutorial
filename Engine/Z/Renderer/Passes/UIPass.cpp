//
// Created by z on 24-1-29.
//

#include "Include/imgui/backends/imgui_impl_glfw.h"
#include "Include/ImGuizmo/ImGuizmo.h"

#include "Z/Renderer/Passes/UIPass.h"
#include "Z/Renderer/Passes/MainCameraPass.h"

#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
    void UIPass::Init(RenderPassInitInfo*info) {
        framebuffer.renderPass=((UIPassInitInfo*)info)->renderpass;
    }

    void UIPass::draw() {
        ImGuiRendererPlatform::GetRenderer()->Begin();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        for(const auto content:ui_Contents) {
            content->OnImGuiRender();
        }
        ImGui::Render();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        ImGuiRendererPlatform::GetRenderer()->End();
    }

    void UIPass::InitUIRenderBackend() {
        ImGuiRendererPlatform::GetRenderer()->InitUIRenderBackend(framebuffer.renderPass,MainCameraPass::_ui_pass_index);
    }
} // Z