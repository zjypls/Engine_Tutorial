//
// Created by z on 24-1-30.
//

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderPipeline.h"

#include "Z/Renderer/Passes/UIPass.h"
#include "Z/Renderer/Passes/MainCameraPass.h"

namespace Z {
    void RenderPipeline::Init(RenderPipelineInitInfo *initInfo) {
        Context=initInfo->graphicContext;
        uiPass=CreateRef<UIPass>();
        mainCameraPass=CreateRef<MainCameraPass>();

        auto cameraPassInitInfo=MainCameraPass::MainCameraPassInitInfo{};
        cameraPassInitInfo.graphicInterface=Context;

        mainCameraPass->Init(&cameraPassInitInfo);

        auto passInfo=UIPassInitInfo{};
        passInfo.renderpass=mainCameraPass->GetRenderPass();
        uiPass->Init(&passInfo);
    }

    void RenderPipeline::draw() {
        ((MainCameraPass*)mainCameraPass.get())->draw(uiPass);
    }

    void RenderPipeline::PushUIContents(ImGuiContent *content) {
        ((UIPass*)uiPass.get())->PushUIContents(content);
    }

    void RenderPipeline::clear() {
        uiPass->clear();
        mainCameraPass->clear();
    }
} // Z