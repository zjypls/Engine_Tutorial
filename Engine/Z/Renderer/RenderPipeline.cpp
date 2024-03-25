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
        auto viewSize=((MainCameraPass*)mainCameraPass.get())->viewPortSize;
        auto width=viewSize.x;
        auto height=viewSize.y;
        Viewport viewports[1] = {{0, 0, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f}};
        Rect2D scissors[1] = {{0, 0, static_cast<uint32>(width), static_cast<uint32>(height)}};
        Rect2D renderArea = {{0, 0}, {static_cast<uint32>(width), static_cast<uint32>(height)}};
        auto currentFrameIndex = Context->GetCurrentFrameIndex();
        static RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.renderPass = ((MainCameraPass*)mainCameraPass.get())->viewportRenderPass;
        renderPassBeginInfo.framebuffer = ((MainCameraPass*)mainCameraPass.get())->viewportFrameBuffer[currentFrameIndex].framebuffer;
        renderPassBeginInfo.renderArea = renderArea;
        renderPassBeginInfo.clearValueCount = 3;
        static ClearValue clearValues[3]={
                {{0.0f, 0.0f, 0.0f, 1.0f}},
                {{{-1}}},
                {{1.0f,0}}
        };
        Context->SetViewPort(viewports[0]);
        Context->SetScissor(scissors[0]);
        renderPassBeginInfo.pClearValues = clearValues;
        Context->BeginRenderPass(renderPassBeginInfo);

        skyboxPass->draw();
        
        Context->EndRenderPass();
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

    void RenderPipeline::Resize() {
        mainCameraPass->Resize();
    }
} // Z