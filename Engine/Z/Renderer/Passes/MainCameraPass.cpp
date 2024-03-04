//
// Created by z on 24-1-31.
//
#include "Z/Core/Log.h"
#include "Z/Renderer/Passes/MainCameraPass.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

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

    void MainCameraPass::clear() {
        RenderPass::clear();
        for(auto buffer:swapchainFrameBuffers) {
            Context->DestroyFrameBuffer(buffer);
            delete buffer;
        }
        Context->DestroyRenderPass(framebuffer.renderPass);
        delete framebuffer.renderPass;
    }

    void MainCameraPass::InitRenderPass() {

        AttachmentReference reference{};
        reference.attachment=0;
        reference.layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        RenderPassCreateInfo info{};
        AttachmentDescription attachmentDescription[_attachment_count];

        auto& swapchainDesc=attachmentDescription[_swapchain_image_index];
        swapchainDesc.format=Context->GetSwapChainInfo().swapchainImageFormat;
        swapchainDesc.samples=SampleCountFlagBits::e1_BIT;
        swapchainDesc.initialLayout=ImageLayout::UNDEFINED;
        swapchainDesc.finalLayout=ImageLayout::PRESENT_SRC;
        swapchainDesc.loadOp=AttachmentLoadOp::CLEAR;
        swapchainDesc.storeOp=AttachmentStoreOp::STORE;
        swapchainDesc.stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        swapchainDesc.stencilStoreOp=AttachmentStoreOp::DONT_CARE;

        info.attachmentCount=_attachment_count;
        info.pAttachments=attachmentDescription;

        SubpassDescription description[_subpass_count];

        auto& uiPassDes=description[_ui_pass_index];
        uiPassDes.colorAttachmentCount=1;
        uiPassDes.pColorAttachments=&reference;
        uiPassDes.inputAttachmentCount=0;
        uiPassDes.pipelineBindPoint=PipelineBindPoint::GRAPHICS;
        uiPassDes.pDepthStencilAttachment=nullptr;

        info.subpassCount=_subpass_count;
        info.pSubpasses=description;

        SubpassDependency dependency[_dependency_count]{};
        auto& uiDependency=dependency[_ui_dependency_index];
        uiDependency.dependencyFlags = DependencyFlags::BY_REGION;
        uiDependency.srcSubpass = SubpassContents::EXTERNAL;
        uiDependency.dstSubpass = SubpassContents::INLINE;
        uiDependency.srcStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT;
        uiDependency.srcAccessMask = AccessFlags::NONE;
        uiDependency.dstStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT;
        uiDependency.dstAccessMask = AccessFlags::COLOR_ATTACHMENT_WRITE;

        info.dependencyCount=_dependency_count;
        info.pDependencies=dependency;

        Context->CreateRenderPass(info,framebuffer.renderPass);
    }

    void MainCameraPass::InitFrameBuffer() {
        swapchainFrameBuffers=Context->CreateDefaultFrameBuffers(framebuffer.renderPass);
    }


    void MainCameraPass::BeginRenderPass() {
        RenderPassBeginInfo beginInfo{};
        Rect2D area{};
        static ClearValue value{.color{0.0f,0,0,0}};
        area.offset={0,0};
        area.extent=Context->GetSwapChainInfo().swapchainExtent;
        beginInfo.renderArea=area;
        beginInfo.clearValueCount=1;
        beginInfo.renderPass=framebuffer.renderPass;
        beginInfo.framebuffer=swapchainFrameBuffers[Context->GetCurrentFrameIndex()];
        beginInfo.pClearValues=&value;
        Context->BeginRenderPass(beginInfo);
    }

    void MainCameraPass::EndRenderPass() {
        Context->EndRenderPass();
    }

} // Z