//
// Created by z on 24-1-29.
//

#ifndef UIPASS_H
#define UIPASS_H

#include <vector>

#include "Z/Core/Log.h"
#include "Z/ImGui/ImGuiContent.h"

#include "Z/Renderer/RenderPass.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {

    struct UIPassInitInfo : RenderPassInitInfo {
        RenderPassInterface* renderpass;
    };

    class Z_API UIPass :public RenderPass{
    public:
        void Init(RenderPassInitInfo*info) override;

        void draw() override;

        void PushUIContents(ImGuiContent* content) {
            Z_CORE_ASSERT(content!=nullptr,"failed to push null content !");
            ui_Contents.push_back(content);
        }

        void clear() override;


    private:
        std::vector<ImGuiContent*> ui_Contents;
        Ref<ImGuiRendererPlatform> imguiRenderer;
    };

} // Z

#endif //UIPASS_H
