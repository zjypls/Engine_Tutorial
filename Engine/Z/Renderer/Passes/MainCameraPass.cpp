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

    void MainCameraPass::clear() {
        RenderPass::clear();
        for(auto buffer:swapchainFrameBuffers) {
            Context->DestroyFrameBuffer(buffer);
        }
        Context->DestroyRenderPass(framebuffer.renderPass);
    }

    void MainCameraPass::InitRenderPass() {

        AttachmentReference reference{};
        reference.attachment=0;
        reference.layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        RenderPassCreateInfo info{};
        info.attachmentCount=_attachment_count;
        AttachmentDescription attachmentDescription[_attachment_count];
        attachmentDescription[_swapchain_image_index].format=Context->GetSwapChainInfo().swapchainImageFormat;
        attachmentDescription[_swapchain_image_index].samples=SampleCountFlagBits::e1_BIT;
        attachmentDescription[_swapchain_image_index].initialLayout=ImageLayout::UNDEFINED;
        attachmentDescription[_swapchain_image_index].finalLayout=ImageLayout::PRESENT_SRC;
        attachmentDescription[_swapchain_image_index].loadOp=AttachmentLoadOp::CLEAR;
        attachmentDescription[_swapchain_image_index].storeOp=AttachmentStoreOp::STORE;
        attachmentDescription[_swapchain_image_index].stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        attachmentDescription[_swapchain_image_index].stencilStoreOp=AttachmentStoreOp::DONT_CARE;
        info.pAttachments=attachmentDescription;
        SubpassDescription description[_subpass_count];
        description[_ui_pass_index].colorAttachmentCount=1;
        description[_ui_pass_index].pColorAttachments=&reference;
        description[_ui_pass_index].inputAttachmentCount=0;
        description[_ui_pass_index].pipelineBindPoint=PipelineBindPoint::GRAPHICS;
        info.subpassCount=_subpass_count;
        info.pSubpasses=description;
        SubpassDependency dependency[_dependency_count]{};
        dependency[_ui_dependency_index].srcSubpass = SubpassContents::EXTERNAL;
        dependency[_ui_dependency_index].dstSubpass = SubpassContents::INLINE;
        dependency[_ui_dependency_index].srcStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT;
        dependency[_ui_dependency_index].srcAccessMask = AccessFlags::NONE;
        dependency[_ui_dependency_index].dstStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT;
        dependency[_ui_dependency_index].dstAccessMask = AccessFlags::COLOR_ATTACHMENT_WRITE;
        info.dependencyCount=_dependency_count;
        info.pDependencies=dependency;

        RenderPassInterface* interface;
        Context->CreateRenderPass(info,interface);
        framebuffer.renderPass=interface;
    }

    void MainCameraPass::InitFrameBuffer() {
        swapchainFrameBuffers=Context->CreateDefaultFrameBuffers(framebuffer.renderPass);
    }


    void MainCameraPass::BeginRenderPass() {
        RenderPassBeginInfo beginInfo{};
        Rect2D area{};
        area.offset={0,0};
        area.extent=Context->GetSwapChainInfo().swapchainExtent;
        beginInfo.renderArea=area;
        beginInfo.clearValueCount=1;
        beginInfo.renderPass=framebuffer.renderPass;
        beginInfo.framebuffer=swapchainFrameBuffers[Context->GetCurrentFrameIndex()];
        ClearValue value{{}};
        beginInfo.pClearValues=&value;
        Context->BeginRenderPass(beginInfo);
    }

    void MainCameraPass::EndRenderPass() {
        Context->EndRenderPass();
    }

} // Z