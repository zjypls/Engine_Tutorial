//
// Created by z on 24-1-31.
//

#include "MainCameraPass.h"

namespace Z {
    void MainCameraPass::Init(RenderPassInitInfo *info) {

        Context=info->graphicInterface;
        framebuffer=RenderPass::Framebuffer{};
        InitRenderPass();
        InitFrameBuffer();

    }

    void MainCameraPass::draw(const Ref<RenderPass> &uiPass) {
        BeginRenderPass();
        uiPass->draw();
        EndRenderPass();
    }

    void MainCameraPass::InitRenderPass() {

    }

    void MainCameraPass::InitFrameBuffer() {
    }


    void MainCameraPass::BeginRenderPass() {
    }

    void MainCameraPass::EndRenderPass() {
        Context->EndRenderPass();
    }

} // Z