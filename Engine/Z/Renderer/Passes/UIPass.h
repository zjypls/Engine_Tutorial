//
// Created by z on 24-1-29.
//

#ifndef UIPASS_H
#define UIPASS_H

#include <vector>

#include "Z/Core/Log.h"
#include "Z/ImGui/ImGuiContent.h"

#include "Z/Renderer/RenderPass.h"

namespace Z {

    class Z_API UIPass :public RenderPass{
    public:
        void Init(RenderPassInitInfo*info) override;

        void draw() override;

        void PushUIContents(ImGuiContent* content) {
            Z_CORE_ASSERT(content!=nullptr,"failed to push null content !");
            ui_Contents.push_back(content);
        }

    private:
        std::vector<ImGuiContent*> ui_Contents;
    };

} // Z

#endif //UIPASS_H
