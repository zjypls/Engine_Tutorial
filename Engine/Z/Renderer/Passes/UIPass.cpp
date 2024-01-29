//
// Created by z on 24-1-29.
//

#include "UIPass.h"

namespace Z {
    void UIPass::Init(RenderPassInitInfo*info) {
    }

    void UIPass::draw() {
        for(const auto content:ui_Contents) {
            content->OnImGuiRender();
        }
    }
} // Z