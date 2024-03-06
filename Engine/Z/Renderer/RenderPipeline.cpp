//
// Created by z on 24-1-30.
//

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderPipeline.h"
#include "Z/Utils/ZUtils.h"
#include "Z/Renderer/Passes/UIPass.h"
#include "Z/Renderer/Passes/MainCameraPass.h"
#include "Z/Renderer/Passes/SkyboxPass.h"

namespace Z {
    void RenderPipeline::Init(RenderPipelineInitInfo *initInfo) {
        Context=initInfo->graphicContext;
        uiPass=CreateRef<UIPass>();
        mainCameraPass=CreateRef<MainCameraPass>();
        auto mainCameraPassPtr=(MainCameraPass*)mainCameraPass.get();

        auto cameraPassInitInfo=MainCameraPass::MainCameraPassInitInfo{};
        cameraPassInitInfo.graphicInterface=Context;

        mainCameraPass->Init(&cameraPassInitInfo);

        auto passInfo=UIPassInitInfo{};
        passInfo.renderpass=mainCameraPass->GetRenderPass();
        uiPass->Init(&passInfo);
        mainCameraPass->PostInit();

        auto skyboxPassInitInfo=SkyboxPassInitInfo{};
        skyboxPassInitInfo.renderpass=mainCameraPassPtr->viewportRenderPass;
        skyboxPassInitInfo.frameBufferCount=Context->GetMaxFramesInFlight();
        skyboxPassInitInfo.framebuffer=mainCameraPassPtr->viewportFrameBuffer.data();
        skyboxPassInitInfo.width=mainCameraPassPtr->viewPortSize.x;
        skyboxPassInitInfo.height=mainCameraPassPtr->viewPortSize.y;
        skyboxPassInitInfo.graphicInterface=Context;
        skyboxPass=CreateRef<SkyboxPass>();
        skyboxPass->Init(&skyboxPassInitInfo);

    }

    void RenderPipeline::draw() {
        skyboxPass->draw();
        ((MainCameraPass*)mainCameraPass.get())->draw(uiPass);
    }

    void RenderPipeline::PushUIContents(ImGuiContent *content) {
        ((UIPass*)uiPass.get())->PushUIContents(content);
    }

    void RenderPipeline::clear() {
        skyboxPass->clear();
        uiPass->clear();
        mainCameraPass->clear();
    }

    void RenderPipeline::SetViewPortSize(uint32 width, uint32 height) {
        ((MainCameraPass*)mainCameraPass.get())->SetViewPortSize(width,height);
        ((SkyboxPass*)skyboxPass.get())->SetViewportSize(width,height);
    }

    void *RenderPipeline::GetViewportFrameBufferDescriptor() {
        return ((MainCameraPass*)mainCameraPass.get())->GetViewportFrameBufferDescriptor();
    }
} // Z