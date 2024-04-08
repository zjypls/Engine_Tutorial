//
// Created by z on 24-1-31.
//
#include "Z/Core/Log.h"
#include "Z/Renderer/Passes/MainCameraPass.h"
#include "Z/ImGui/ImGuiRendererPlatform.h"

namespace Z {
    constexpr static auto depthStencilFormat = Format::D32_SFLOAT;
    constexpr static auto indexFormat = Format::R32_SINT;
    void MainCameraPass::Init(RenderPassInitInfo *info) {

        Context=info->graphicInterface;
        framebuffer=RenderPass::Framebuffer{};
        InitRenderPass();
        InitFrameBuffer();
        InitViewportRenderPass();
        InitViewportFrameBuffer();
    }

    void MainCameraPass::draw(const Ref<RenderPass> &uiPass) {
        BeginRenderPass();
        uiPass->draw();
        EndRenderPass();
    }

    void MainCameraPass::clear() {
        RenderPass::clear();
        DestroyViewportFrameBuffer();
        Context->DestroyRenderPass(viewportRenderPass);
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

    void MainCameraPass::InitViewportFrameBuffer() {
        viewportFrameBuffer.resize(swapchainFrameBuffers.size());

        ImageInfo imageInfo{};
        imageInfo.extent={viewPortSize.x,viewPortSize.y,1};
        imageInfo.initialLayout=ImageLayout::UNDEFINED;
        imageInfo.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
        imageInfo.mipMapLevels=1;
        imageInfo.arrayLayers=1;
        imageInfo.tilling=ImageTiling::OPTIMAL;
        imageInfo.sampleCount=SampleCountFlagBits::e1_BIT;

        ImageViewInfo imageViewInfo{};
        imageViewInfo.viewType=ImageViewType::e2D;
        imageViewInfo.subresourceRange.baseArrayLayer=0;
        imageViewInfo.subresourceRange.layerCount=1;
        imageViewInfo.subresourceRange.baseMipLevel=0;
        imageViewInfo.subresourceRange.levelCount=1;

        FramebufferInfo framebufferCreateInfo{};
        framebufferCreateInfo.attachmentCount=_viewport_attachment_count;
        framebufferCreateInfo.renderPass=viewportRenderPass;
        framebufferCreateInfo.extent={viewPortSize.x,viewPortSize.y};
        framebufferCreateInfo.layers=1;

        for(auto & framebuffer : viewportFrameBuffer){
            framebuffer.renderPass=viewportRenderPass;
            framebuffer.width=viewPortSize.x;
            framebuffer.height=viewPortSize.y;
            framebuffer.attachments.resize(_viewport_attachment_count);

            auto& colorAttachment=framebuffer.attachments[_viewport_color_attachment_index];
            auto& goIndexAttachment=framebuffer.attachments[_viewport_goindex_attachment_index];
            auto& depthAttachment=framebuffer.attachments[_viewport_depth_attachment_index];

            colorAttachment.format=viewportColorFormat;
            goIndexAttachment.format=indexFormat;
            depthAttachment.format=depthStencilFormat;

            imageInfo.format=viewportColorFormat;
            imageInfo.usageFlag=ImageUsageFlag::COLOR_ATTACHMENT|ImageUsageFlag::SAMPLED;
            Context->CreateImage(imageInfo,colorAttachment.image,colorAttachment.memory);

            imageInfo.format=indexFormat;
            imageInfo.usageFlag=ImageUsageFlag::COLOR_ATTACHMENT;
            Context->CreateImage(imageInfo,goIndexAttachment.image,goIndexAttachment.memory);

            imageInfo.format=depthStencilFormat;
            imageInfo.usageFlag=ImageUsageFlag::DEPTH_STENCIL_ATTACHMENT;
            Context->CreateImage(imageInfo,depthAttachment.image,depthAttachment.memory);

            imageViewInfo.format=viewportColorFormat;
            imageViewInfo.image=colorAttachment.image;
            imageViewInfo.subresourceRange.aspectMask=ImageAspectFlag::COLOR;
            Context->CreateImageView(imageViewInfo,colorAttachment.view);

            imageViewInfo.format=indexFormat;
            imageViewInfo.image=goIndexAttachment.image;
            Context->CreateImageView(imageViewInfo,goIndexAttachment.view);

            imageViewInfo.format=depthStencilFormat;
            imageViewInfo.image=depthAttachment.image;
            imageViewInfo.subresourceRange.aspectMask=ImageAspectFlag::DEPTH ;
            Context->CreateImageView(imageViewInfo,depthAttachment.view);

            ImageView* attachments[_viewport_attachment_count];
            attachments[_viewport_color_attachment_index]=colorAttachment.view;
            attachments[_viewport_goindex_attachment_index]=goIndexAttachment.view;
            attachments[_viewport_depth_attachment_index]=depthAttachment.view;

            framebufferCreateInfo.pAttachments=attachments;
            Context->CreateFrameBuffer(framebufferCreateInfo, framebuffer.framebuffer);
        }
    }


    void MainCameraPass::BeginRenderPass() {
        RenderPassBeginInfo beginInfo{};
        Rect2D area{};
        static ClearValue value{{0.0f,0,0,0}};
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

    void MainCameraPass::DestroyViewportFrameBuffer(){
        for(const auto& buffer:viewportFrameBuffer) {
            for(auto view:buffer.attachments){
                Context->DestroyImage(view.image,view.memory, view.view);
            }
            Context->DestroyFrameBuffer(buffer.framebuffer);
        }
    }

    void MainCameraPass::SetViewPortSize(uint32 width, uint32 height) {
        if(viewPortSize.x==width&&viewPortSize.y==height)return;
        for(auto& set:viewPortDescriptorSets){
            Context->FreeDescriptorSet(&set);
        }
        DestroyViewportFrameBuffer();
        viewPortSize.x=width;
        viewPortSize.y=height;
        InitViewportFrameBuffer();
        PostInit();
    }

    void MainCameraPass::InitViewportRenderPass() {

        AttachmentReference viewportReference[_viewport_attachment_count]{};
        viewportReference[_viewport_color_attachment_index].attachment=_viewport_color_attachment_index;
        viewportReference[_viewport_color_attachment_index].layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        viewportReference[_viewport_goindex_attachment_index].attachment=_viewport_goindex_attachment_index;
        viewportReference[_viewport_goindex_attachment_index].layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        viewportReference[_viewport_depth_attachment_index].attachment=_viewport_depth_attachment_index;
        viewportReference[_viewport_depth_attachment_index].layout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        AttachmentDescription viewportAttachmentDescription[_viewport_attachment_count]{};
        auto& colorAttachmentDes=viewportAttachmentDescription[_viewport_color_attachment_index];
        auto& goIndexAttachmentDes=viewportAttachmentDescription[_viewport_goindex_attachment_index];
        auto& depthAttachmentDes=viewportAttachmentDescription[_viewport_depth_attachment_index];

        colorAttachmentDes.format=viewportColorFormat;
        colorAttachmentDes.samples=SampleCountFlagBits::e1_BIT;
        colorAttachmentDes.initialLayout=ImageLayout::UNDEFINED;
        colorAttachmentDes.finalLayout=ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        colorAttachmentDes.loadOp=AttachmentLoadOp::CLEAR;
        colorAttachmentDes.storeOp=AttachmentStoreOp::STORE;
        colorAttachmentDes.stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        colorAttachmentDes.stencilStoreOp=AttachmentStoreOp::DONT_CARE;

        goIndexAttachmentDes.format=indexFormat;
        goIndexAttachmentDes.samples=SampleCountFlagBits::e1_BIT;
        goIndexAttachmentDes.initialLayout=ImageLayout::UNDEFINED;
        goIndexAttachmentDes.finalLayout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        goIndexAttachmentDes.loadOp=AttachmentLoadOp::CLEAR;
        goIndexAttachmentDes.storeOp=AttachmentStoreOp::STORE;
        goIndexAttachmentDes.stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        goIndexAttachmentDes.stencilStoreOp=AttachmentStoreOp::DONT_CARE;

        depthAttachmentDes.format=depthStencilFormat;
        depthAttachmentDes.samples=SampleCountFlagBits::e1_BIT;
        depthAttachmentDes.initialLayout=ImageLayout::UNDEFINED;
        depthAttachmentDes.finalLayout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentDes.loadOp=AttachmentLoadOp::CLEAR;
        depthAttachmentDes.storeOp=AttachmentStoreOp::DONT_CARE;
        depthAttachmentDes.stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        depthAttachmentDes.stencilStoreOp=AttachmentStoreOp::DONT_CARE;

        SubpassDescription viewportDescription[_viewport_subpass_count]{};
        auto& skyboxSubpassDes=viewportDescription[_viewport_skybox_index];
        skyboxSubpassDes.colorAttachmentCount=_viewport_attachment_count-1;
        skyboxSubpassDes.pColorAttachments=viewportReference;
        skyboxSubpassDes.inputAttachmentCount=0;
        skyboxSubpassDes.pipelineBindPoint=PipelineBindPoint::GRAPHICS;
        skyboxSubpassDes.pDepthStencilAttachment=viewportReference+_viewport_depth_attachment_index;

        SubpassDependency viewportDependency[_viewport_dependency_count]{};
        auto& skyboxDependency=viewportDependency[_viewport_skybox_dependency_index];
        skyboxDependency.dependencyFlags = DependencyFlags::BY_REGION;
        skyboxDependency.srcSubpass = SubpassContents::EXTERNAL;
        skyboxDependency.dstSubpass = SubpassContents::INLINE;
        skyboxDependency.srcStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT | PipelineStageFlags::EARLY_FRAGMENT_TESTS;
        skyboxDependency.srcAccessMask = AccessFlags::NONE;
        skyboxDependency.dstStageMask =PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT| PipelineStageFlags::EARLY_FRAGMENT_TESTS;
        skyboxDependency.dstAccessMask = AccessFlags::COLOR_ATTACHMENT_WRITE | AccessFlags::DEPTH_STENCIL_ATTACHMENT_WRITE;

        RenderPassCreateInfo viewportPassInfo{};
        viewportPassInfo.attachmentCount=_viewport_attachment_count;
        viewportPassInfo.pAttachments=viewportAttachmentDescription;
        viewportPassInfo.subpassCount=_viewport_subpass_count;
        viewportPassInfo.pSubpasses=viewportDescription;
        viewportPassInfo.dependencyCount=_viewport_dependency_count;
        viewportPassInfo.pDependencies=viewportDependency;

        Context->CreateRenderPass(viewportPassInfo,viewportRenderPass);
    }

    void MainCameraPass::PostInit() {
        viewPortDescriptorSets.resize(viewportFrameBuffer.size());
        auto sampler=Context->GetDefaultSampler(SamplerType::Linear);
        for(int i=0;i<viewPortDescriptorSets.size();++i){
            ImGuiRendererPlatform::GetRenderer()->CreateTextureForImGui(sampler,viewportFrameBuffer[i].attachments[_viewport_color_attachment_index].view,
                                                                        ImageLayout::SHADER_READ_ONLY_OPTIMAL,viewPortDescriptorSets[i]);
        }
    }

    void *MainCameraPass::GetViewportFrameBufferDescriptor() {
        return viewPortDescriptorSets[Context->GetCurrentFrameIndex()];
    }

    void MainCameraPass::Resize() {
        for(auto buffer:swapchainFrameBuffers) {
            Context->DestroyFrameBuffer(buffer);
        }
        swapchainFrameBuffers.clear();
        swapchainFrameBuffers=Context->CreateDefaultFrameBuffers(framebuffer.renderPass);
    }

} // Z