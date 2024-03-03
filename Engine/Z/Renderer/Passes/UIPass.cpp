//
// Created by z on 24-1-29.
//

#include "Include/imgui/backends/imgui_impl_glfw.h"
#include "Include/ImGuizmo/ImGuizmo.h"

#include "Z/Renderer/Passes/UIPass.h"
#include "Z/Renderer/Passes/MainCameraPass.h"


namespace Z {
    namespace Tool{
        void ConfigureTheme(ImGuiStyle &style) {
            //Dark Theme
            auto &colors = style.Colors;
            colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

            // Headers
            colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
            colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
            colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

            // Buttons
            colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
            colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
            colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

            // Frame BG
            colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
            colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
            colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

            // Tabs
            colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
            colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
            colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
            colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
            colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

            // Title
            colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
            colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

        }

        void InitImGui() {
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = nullptr;//"Assets/Configs/editorLayout.ini";
            ImGui::LoadIniSettingsFromDisk("Assets/Configs/editorLayout.ini");


            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      //Necessary for docking

            io.Fonts->AddFontFromFileTTF("Assets/fonts/Oxygen/Oxygen-Bold.ttf", 18.0f);
            io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/fonts/Oxygen/Oxygen-Regular.ttf", 18.0f);

            ImGuiStyle &style = ImGui::GetStyle();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            ConfigureTheme(style);


            io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
            io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
            //Set imgui block events
            //io.SetAppAcceptingEvents(true);
        }
    }

    void UIPass::Init(RenderPassInitInfo*info) {
        framebuffer.renderPass=((UIPassInitInfo*)info)->renderpass;
        Tool::InitImGui();
        ImGuiRendererPlatform::Init();
        imguiRenderer=ImGuiRendererPlatform::GetRenderer();
        imguiRenderer->PlatformInit();
        imguiRenderer->InitUIRenderBackend(framebuffer.renderPass,MainCameraPass::_ui_pass_index);
    }

    void UIPass::draw() {
        imguiRenderer->Begin();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        for(const auto content:ui_Contents) {
            content->OnImGuiRender();
        }
        ImGui::Render();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        imguiRenderer->End();
    }

    void UIPass::clear() {
        Z_CORE_WARN("UI Pass clear !");
        imguiRenderer->Shutdown();
        imguiRenderer=nullptr;
    }
} // Z