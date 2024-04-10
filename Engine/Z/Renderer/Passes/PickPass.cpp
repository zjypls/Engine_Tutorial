//
// Created by z on 2024/4/9.
//
#include "Z/Core/AssetsSystem.h"

#include "Z/Renderer/Passes/PickPass.h"
#include "Z/Renderer/RenderManager.h"
#include "Z/Renderer/RenderResource.h"

namespace Z {
    void PickPass::Init(RenderPassInitInfo *info) {
        Context=info->graphicInterface;
        InitRenderPass();
        InitFrameBuffer();
        InitPipeline();
    }

    void PickPass::InitRenderPass() {

        AttachmentDescription description[2]{};
        description[0].format=Format::R32_SINT;
        description[0].initialLayout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        description[0].finalLayout=ImageLayout::TRANSFER_SRC_OPTIMAL;
        description[0].stencilStoreOp=AttachmentStoreOp::DONT_CARE;
        description[0].stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        description[0].loadOp=AttachmentLoadOp::CLEAR;
        description[0].storeOp=AttachmentStoreOp::STORE;
        description[0].samples=SampleCountFlagBits::e1_BIT;
        description[1].format=Format::D32_SFLOAT_S8_UINT;
        description[1].initialLayout=ImageLayout::UNDEFINED;
        description[1].finalLayout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        description[1].stencilStoreOp=AttachmentStoreOp::DONT_CARE;
        description[1].stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        description[1].loadOp=AttachmentLoadOp::CLEAR;
        description[1].storeOp=AttachmentStoreOp::DONT_CARE;
        description[1].samples=SampleCountFlagBits::e1_BIT;

        AttachmentReference attachmentReference[2]{};
        attachmentReference[0].layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        attachmentReference[0].attachment=0;
        attachmentReference[1].layout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentReference[1].attachment=1;

        SubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint=PipelineBindPoint::GRAPHICS;
        subpassDescription.pDepthStencilAttachment=attachmentReference+1;
        subpassDescription.colorAttachmentCount=1;
        subpassDescription.pColorAttachments=attachmentReference;
        subpassDescription.pInputAttachments= nullptr;

        RenderPassCreateInfo createInfo{};
        createInfo.pAttachments=description;
        createInfo.attachmentCount=2;
        createInfo.subpassCount=1;
        createInfo.pSubpasses=&subpassDescription;
        createInfo.dependencyCount=0;

        Context->CreateRenderPass(createInfo,framebuffer.renderPass);
    }
    void PickPass::InitPipeline() {
        renderPipelines.resize(2);
        auto shaderRes = AssetsSystem::Load<ShaderRes>("Assets/Shaders/pick.glsl");
        std::vector<DescriptorSetLayout*> layouts;
        Context->CreateGraphicPipeline(shaderRes->source,shaderRes->stages,renderPipelines[0].pipeline,framebuffer.renderPass,layouts,renderPipelines[0].layout,
                                       nullptr , false);

        shaderRes = AssetsSystem::Load<ShaderRes>("Assets/Shaders/pick-noVertexBlending.glsl");
        Context->CreateGraphicPipeline(shaderRes->source,shaderRes->stages,renderPipelines[1].pipeline,framebuffer.renderPass,
                                       noVertexBlendingLay,renderPipelines[1].layout,nullptr , false);

        descriptors.resize(layouts.size());
        for(int i=0;i<layouts.size();++i){
            descriptors[i].layout=layouts[i];
        }
    }

    void PickPass::draw() {
        static ClearValue pickClearValue[2]{{-1},{1}};
        Rect2D scissor{{0,0},{viewPortSize.x,viewPortSize.y}};
        Viewport viewport{0,0,static_cast<float>(viewPortSize.x),static_cast<float>(viewPortSize.y),0,1};

        auto buffer =Context->BeginOnceCommandSubmit();

        auto indices = Context->GetQueueFamilyIndices();

        ImageMemoryBarrier memoryBarrier{};
        memoryBarrier.oldLayout=ImageLayout::UNDEFINED;
        memoryBarrier.newLayout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        memoryBarrier.image=framebuffer.attachments[0].image;
        memoryBarrier.dstQueueFamilyIndex=indices.graphics.value();
        memoryBarrier.srcQueueFamilyIndex=indices.graphics.value();
        memoryBarrier.dstAccessMask=AccessFlags::COLOR_ATTACHMENT_WRITE;
        memoryBarrier.subresourceRange= {ImageAspectFlag::COLOR,0,1,0,1};

        Context->PipelineBarrier(buffer,PipelineStageFlags::ALL_COMMANDS,PipelineStageFlags::ALL_COMMANDS,DependencyFlags::BY_REGION,
                                 0, nullptr,0, nullptr,
                                 1,&memoryBarrier);

        auto set0=RenderResource::GetInnerDescriptorSets()[0];
        auto& meshMap= RenderResource::GetMaterialMap();
        auto& idMap = RenderResource::GetGOIndexMap();
        auto& setMap = RenderResource::GetGODescriptorMap();

        auto beginInfo = RenderPassBeginInfo{};
        beginInfo.renderPass=framebuffer.renderPass;
        beginInfo.framebuffer=framebuffer.framebuffer;
        beginInfo.renderArea={{0,0},{viewPortSize.x,viewPortSize.y}};
        beginInfo.clearValueCount=2;
        beginInfo.pClearValues=pickClearValue;
        beginInfo.pNext= nullptr;
        Context->BeginRenderPass(buffer,beginInfo);
        Context->SetScissor(buffer,scissor);
        Context->SetViewPort(buffer,viewport);

        for(auto&[mat,val]:meshMap){
            for(auto&[id,mesh]:val){
                auto meshRes = AssetsSystem::Load<MeshRes>(mesh);
                std::vector sets{set0};
                if(meshRes->animator){
                    sets.push_back(setMap[id][RenderManager::BoneNodeTransformSet-2]);
                    Context->BindPipeline(buffer,PipelineBindPoint::GRAPHICS,renderPipelines[0].pipeline);
                    Context->PushConstant(buffer,renderPipelines[0].layout,ShaderStageFlag::VERTEX,0,sizeof(int),&idMap[id]);
                    Context->BindDescriptorSets(buffer,PipelineBindPoint::GRAPHICS,renderPipelines[0].layout,0,sets);
                }else{
                    Context->BindPipeline(buffer,PipelineBindPoint::GRAPHICS,renderPipelines[1].pipeline);
                    Context->PushConstant(buffer,renderPipelines[1].layout,ShaderStageFlag::VERTEX,0,sizeof(int),&idMap[id]);
                    Context->BindDescriptorSets(buffer,PipelineBindPoint::GRAPHICS,renderPipelines[1].layout,0,sets);
                }
                Context->BindVertexBuffer(buffer,meshRes->vertexBuffer,0,meshRes->vertexBufferCount);
                Context->BindIndexBuffer(buffer,meshRes->indexBuffer);
                Context->DrawIndexed(buffer,meshRes->indicesCount,1,0,0,0);
            }
        }
        Context->EndRenderPass(buffer);
        Context->EndOnceSubmit(buffer);
    }

    void PickPass::InitFrameBuffer() {
        framebuffer.attachments.resize(2);
        viewPortSize = RenderManager::GetViewportSize();
        auto &indexAttach=framebuffer.attachments[0];
        auto &depthAttach = framebuffer.attachments[1];

        ImageInfo indexImage{};
        indexImage.initialLayout=ImageLayout::UNDEFINED;
        indexImage.format=Format::R32_SINT;
        indexImage.usageFlag=ImageUsageFlag::COLOR_ATTACHMENT|ImageUsageFlag::TRANSFER_SRC;
        indexImage.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
        indexImage.extent={viewPortSize.x,viewPortSize.y,1};
        indexImage.tilling=ImageTiling::OPTIMAL;
        indexImage.arrayLayers=1;
        indexImage.sampleCount=SampleCountFlagBits::e1_BIT;
        indexImage.mipMapLevels=1;

        Context->CreateImage(indexImage,indexAttach.image,indexAttach.memory);

        ImageViewInfo viewInfo{};
        viewInfo.image=indexAttach.image;
        viewInfo.format=Format::R32_SINT;
        viewInfo.subresourceRange.baseArrayLayer=0;
        viewInfo.subresourceRange.layerCount=1;
        viewInfo.subresourceRange.baseMipLevel=0;
        viewInfo.subresourceRange.levelCount=1;
        viewInfo.subresourceRange.aspectMask=ImageAspectFlag::COLOR;
        viewInfo.components.a=ComponentSwizzle::IDENTITY;
        viewInfo.components.r=ComponentSwizzle::IDENTITY;
        viewInfo.components.g=ComponentSwizzle::IDENTITY;
        viewInfo.components.b=ComponentSwizzle::IDENTITY;
        viewInfo.viewType=ImageViewType::e2D;

        Context->CreateImageView(viewInfo,indexAttach.view);

        ImageInfo depthInfo{};
        depthInfo.format=Format::D32_SFLOAT_S8_UINT;
        depthInfo.sampleCount=SampleCountFlagBits::e1_BIT;
        depthInfo.arrayLayers=1;
        depthInfo.usageFlag=ImageUsageFlag::DEPTH_STENCIL_ATTACHMENT;
        depthInfo.tilling=ImageTiling::OPTIMAL;
        depthInfo.extent={viewPortSize.x,viewPortSize.y,1};
        depthInfo.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
        depthInfo.mipMapLevels=1;

        Context->CreateImage(depthInfo,depthAttach.image,depthAttach.memory);

        ImageViewInfo depthViewInfo{};
        depthViewInfo.subresourceRange.aspectMask=ImageAspectFlag::DEPTH;
        depthViewInfo.subresourceRange.levelCount=1;
        depthViewInfo.subresourceRange.baseMipLevel=0;
        depthViewInfo.subresourceRange.baseArrayLayer=0;
        depthViewInfo.subresourceRange.layerCount=1;
        depthViewInfo.components.a=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.r=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.g=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.b=ComponentSwizzle::IDENTITY;
        depthViewInfo.image=depthAttach.image;
        depthViewInfo.format=Format::D32_SFLOAT_S8_UINT;
        depthViewInfo.viewType=ImageViewType::e2D;

        Context->CreateImageView(depthViewInfo,depthAttach.view);

        FramebufferInfo framebufferInfo{};
        framebufferInfo.extent={viewPortSize.x,viewPortSize.y};
        framebufferInfo.renderPass=framebuffer.renderPass;
        framebufferInfo.layers=1;
        framebufferInfo.attachmentCount=2;
        ImageView* attachments[2]{indexAttach.view,depthAttach.view};
        framebufferInfo.pAttachments=attachments;

        Context->CreateFrameBuffer(framebufferInfo,framebuffer.framebuffer);
    }

    void PickPass::clear() {
        clearFramebuffer();
        for(auto val:renderPipelines){
            Context->DestroyPipeline(val.pipeline);
            Context->DestroyPipelineLayout(val.layout);
        }
        for(auto& [layout]:descriptors){
            Context->DestroyDescriptorSetLayout(layout);
        }
        for(auto lay:noVertexBlendingLay){
            Context->DestroyDescriptorSetLayout(lay);
        }
        noVertexBlendingLay.clear();
        descriptors.clear();
        Context->DestroyRenderPass(framebuffer.renderPass);
    }

    int PickPass::GetPickValue(int x, int y) {
        BufferInfo info{};
        info.size=viewPortSize.x*viewPortSize.y*4;
        info.properties=MemoryPropertyFlag::HOST_COHERENT|MemoryPropertyFlag::HOST_VISIBLE;
        info.usage=BufferUsageFlag::TRANSFER_DST;
        Buffer* buffer;
        DeviceMemory* memory;
        Context->CreateBuffer(info,buffer,memory);
        Context->CopyImageToBuffer(framebuffer.attachments[0].image,buffer,ImageLayout::TRANSFER_SRC_OPTIMAL,
                                   {{0,0},{viewPortSize.x,viewPortSize.y}});
        void* pixels;
        Context->MapMemory(memory,0,info.size,pixels);
        Z_CORE_ASSERT(pixels,"failed to map memory !");
        auto res = *((int32*)pixels+viewPortSize.x*y+x);
        Z_CORE_WARN("pick pos : ({0},{1}), Pick val : {2}",x,y,res);
        Context->UnMapMemory(memory);
        Context->DestroyBuffer(buffer,memory);
        return res;
    }

    void PickPass::clearFramebuffer() {
        for(auto attachment:framebuffer.attachments)
            Context->DestroyImage(attachment.image,attachment.memory,attachment.view);
        framebuffer.attachments.clear();
        Context->DestroyFrameBuffer(framebuffer.framebuffer);
    }

    void PickPass::SetViewportSize(uint32 width, uint32 height) {
        if(width==viewPortSize.x&&height==viewPortSize.y)return;
        viewPortSize={width,height};
        clearFramebuffer();
        InitFrameBuffer();
    }

} // Z