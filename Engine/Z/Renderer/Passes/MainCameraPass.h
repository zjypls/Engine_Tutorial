//
// Created by z on 24-1-31.
//

#ifndef MAINCAMERAPASS_H
#define MAINCAMERAPASS_H

#include "Z/Renderer/RenderPass.h"

namespace Z {
    class Z_API MainCameraPass final : public RenderPass {
    public:
        struct MainCameraPassInitInfo : RenderPassInitInfo {

        };
        enum SubPassIndex {
            _ui_pass_index=0,
            _subpass_count
        };
        enum AttachmentIndex {
            _swapchain_image_index=0,
            _attachment_count
        };
        enum SubpassDependencyIndex {
            _ui_dependency_index=0,
            _dependency_count
        };
        void Init(RenderPassInitInfo *info) override;
    private:
    };
} // Z

#endif //MAINCAMERAPASS_H
