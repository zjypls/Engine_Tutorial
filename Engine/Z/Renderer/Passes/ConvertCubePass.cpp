//
// Created by z on 2024/4/6.
//
#include "Z/Core/AssetsSystem.h"

#include "Z/Renderer/Passes/ConvertCubePass.h"

namespace Z {
    void ConvertCubePass::Init(RenderPassInitInfo *info) {
        auto toolInfo=(ConvertCubePassInitInfo*)info;
        Z_CORE_ASSERT(!toolInfo->toolShaderPath.empty(),"Init shader with empty path !");
        Context=info->graphicInterface;
        shaderPath=toolInfo->toolShaderPath;
        InitBuffer();
        InitRenderPass();
        InitPipeline();
        InitDescriptorSets();
    }

    void ConvertCubePass::InitRenderPass() {

        RenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.attachmentCount=2;
        renderPassCreateInfo.dependencyCount=0;
        renderPassCreateInfo.subpassCount=1;
        AttachmentDescription description[2]{};
        description[0].finalLayout=ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        description[0].initialLayout=ImageLayout::UNDEFINED;
        description[0].format=Format::R32G32B32A32_SFLOAT;
        description[0].loadOp=AttachmentLoadOp::CLEAR;
        description[0].storeOp=AttachmentStoreOp::STORE;
        description[0].samples=SampleCountFlagBits::e1_BIT;
        description[0].stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        description[0].stencilStoreOp=AttachmentStoreOp::DONT_CARE;
        description[1].finalLayout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        description[1].initialLayout=ImageLayout::UNDEFINED;
        description[1].format=Format::D32_SFLOAT_S8_UINT;
        description[1].loadOp=AttachmentLoadOp::CLEAR;
        description[1].storeOp=AttachmentStoreOp::STORE;
        description[1].samples=SampleCountFlagBits::e1_BIT;
        description[1].stencilLoadOp=AttachmentLoadOp::DONT_CARE;
        description[1].stencilStoreOp=AttachmentStoreOp::DONT_CARE;

        renderPassCreateInfo.pAttachments=description;

        AttachmentReference reference{};
        reference.layout=ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
        reference.attachment=0;

        AttachmentReference depthRef{};
        depthRef.attachment=1;
        depthRef.layout=ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        SubpassDescription subpassDescription{};
        subpassDescription.colorAttachmentCount=1;
        subpassDescription.inputAttachmentCount=0;
        subpassDescription.pColorAttachments=&reference;
        subpassDescription.pDepthStencilAttachment=&depthRef;
        subpassDescription.pipelineBindPoint=PipelineBindPoint::GRAPHICS;

        renderPassCreateInfo.pSubpasses=&subpassDescription;

        Context->CreateRenderPass(renderPassCreateInfo,framebuffer.renderPass);
    }

    void ConvertCubePass::InitPipeline() {
        renderPipelines.resize(1);
        auto shaderRes = AssetsSystem::Load<ShaderRes>(shaderPath);
        std::vector<DescriptorSetLayout*> layouts;
        Context->CreateGraphicPipeline(shaderRes->source,shaderRes->stages,renderPipelines[0].pipeline,framebuffer.renderPass,
                                       layouts,renderPipelines[0].layout,nullptr, false);
        descriptors.resize(layouts.size());
        for(int i=0;i<layouts.size();++i){
            descriptors[i].layout=layouts[i];
        }
    }

    void ConvertCubePass::clear() {
        clearFrameBuffer();
        for(auto&[layout,pipeline]:renderPipelines){
            Context->DestroyPipeline(pipeline);
            Context->DestroyPipelineLayout(layout);
        }
        renderPipelines.clear();
        for(auto set:descriptorSets){
            Context->FreeDescriptorSet(set);
        }
        descriptorSets.clear();
        for(auto& layout:descriptors){
            Context->DestroyDescriptorSetLayout(layout.layout);
        }
        descriptors.clear();
        Context->DestroyRenderPass(framebuffer.renderPass);
        Context->DestroyBuffer(mvBuffer,mvMemory);
        Context = nullptr;
    }

    void ConvertCubePass::InitDescriptorSets() {
        descriptorSets.resize(1);
        DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.pSetLayouts=descriptors[0].layout;
        allocateInfo.descriptorSetCount=1;
        Context->AllocateDescriptorSet(allocateInfo,descriptorSets[0]);

        DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer=mvBuffer;
        bufferInfo.offset=0;
        bufferInfo.range=sizeof(glm::mat4)*7;

        WriteDescriptorSet writeDescriptorSet[1]{};
        writeDescriptorSet[0].descriptorType=DescriptorType::STORAGE_BUFFER;
        writeDescriptorSet[0].descriptorCount=1;
        writeDescriptorSet[0].dstSet=descriptorSets[0];
        writeDescriptorSet[0].dstArrayElement=0;
        writeDescriptorSet[0].pBufferInfo=&bufferInfo;
        writeDescriptorSet[0].pImageInfo= nullptr;
        writeDescriptorSet[0].pTexelBufferView= nullptr;

        Context->WriteDescriptorSets(writeDescriptorSet,1);
    }

    void ConvertCubePass::InitBuffer() {
        BufferInfo info{};
        info.size=sizeof(glm::mat4)*7;
        info.usage=BufferUsageFlag::STORAGE_BUFFER;
        info.properties=MemoryPropertyFlag::DEVICE_LOCAL;
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
        projectionMatrix[1][1]*=-1;

        glm::mat4 vpMatrix[] = {
                projectionMatrix,
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f,  1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f,  1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f,  1.0f,  0.0f))
        };

        Context->CreateBuffer(info,mvBuffer,mvMemory,vpMatrix);
    }

    void ConvertCubePass::SetArgs(const std::string &path) {
        if(path.empty() || sourcePath==path)return;
        sourcePath=path;
        auto source=AssetsSystem::Load<Texture2D>(path);
        sourceImage=source->image;
        sourceImageMemory=source->memory;
        sourceImageView=source->imageView;
        UpdateSourceImageSet();
        if(width==source->width/4)return;
        clearFrameBuffer();
        width=source->width/4;
        height=source->width/4;
        InitFrameBuffer();
    }

    void ConvertCubePass::UpdateSourceImageSet() {
        DescriptorImageInfo imageInfo{};
        imageInfo.imageView=sourceImageView;
        imageInfo.imageLayout=ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        imageInfo.sampler=Context->GetDefaultSampler(SamplerType::Linear);

        WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.pImageInfo=&imageInfo;
        writeDescriptorSet.dstBinding=1;
        writeDescriptorSet.descriptorCount=1;
        writeDescriptorSet.dstArrayElement=0;
        writeDescriptorSet.dstSet=descriptorSets[0];
        writeDescriptorSet.descriptorType=DescriptorType::COMBINED_IMAGE_SAMPLER;

        Context->WriteDescriptorSets(&writeDescriptorSet,1);
    }

    void ConvertCubePass::InitFrameBuffer() {
        ImageInfo cubeInfo{};
        cubeInfo.format=Format::R32G32B32A32_SFLOAT;
        cubeInfo.initialLayout=ImageLayout::UNDEFINED;
        cubeInfo.arrayLayers=6;
        cubeInfo.extent={width,width,1};
        cubeInfo.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
        cubeInfo.tilling=ImageTiling::OPTIMAL;
        cubeInfo.mipMapLevels=1;
        cubeInfo.sampleCount=SampleCountFlagBits::e1_BIT;
        cubeInfo.usageFlag=ImageUsageFlag::COLOR_ATTACHMENT|ImageUsageFlag::SAMPLED;
        cubeInfo.createFlag=ImageCreateFlag::eCUBE_COMPATIBLE;

        Context->CreateImage(cubeInfo,cubeImage,cubeMemory);

        ImageViewInfo cubeViewInfo{};
        cubeViewInfo.format=Format::R32G32B32A32_SFLOAT;
        cubeViewInfo.image=cubeImage;
        cubeViewInfo.components.a=ComponentSwizzle::IDENTITY;
        cubeViewInfo.components.r=ComponentSwizzle::IDENTITY;
        cubeViewInfo.components.g=ComponentSwizzle::IDENTITY;
        cubeViewInfo.components.b=ComponentSwizzle::IDENTITY;
        cubeViewInfo.subresourceRange.aspectMask=ImageAspectFlag::COLOR;
        cubeViewInfo.subresourceRange.baseArrayLayer=0;
        cubeViewInfo.subresourceRange.levelCount=1;
        cubeViewInfo.subresourceRange.baseArrayLayer=0;
        cubeViewInfo.subresourceRange.layerCount=6;
        cubeViewInfo.viewType=ImageViewType::eCUBE;

        Context->CreateImageView(cubeViewInfo,cubeView);

        framebuffer.width=width;
        framebuffer.height=width;


        ImageInfo depthInfo{};
        depthInfo.format=Format::D32_SFLOAT_S8_UINT;
        depthInfo.sampleCount=SampleCountFlagBits::e1_BIT;
        depthInfo.mipMapLevels=1;
        depthInfo.memoryPropertyFlag=MemoryPropertyFlag::DEVICE_LOCAL;
        depthInfo.arrayLayers=1;
        depthInfo.tilling=ImageTiling::OPTIMAL;
        depthInfo.extent={width,width,1};
        depthInfo.initialLayout=ImageLayout::UNDEFINED;
        depthInfo.usageFlag=ImageUsageFlag::DEPTH_STENCIL_ATTACHMENT;


        ImageViewInfo depthViewInfo{};
        depthViewInfo.subresourceRange.aspectMask=ImageAspectFlag::DEPTH;
        depthViewInfo.subresourceRange.baseArrayLayer=0;
        depthViewInfo.subresourceRange.layerCount=1;
        depthViewInfo.subresourceRange.levelCount=1;
        depthViewInfo.subresourceRange.baseMipLevel=0;
        depthViewInfo.components.a=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.r=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.g=ComponentSwizzle::IDENTITY;
        depthViewInfo.components.b=ComponentSwizzle::IDENTITY;
        depthViewInfo.format=Format::D32_SFLOAT_S8_UINT;
        depthViewInfo.viewType=ImageViewType::e2D;

        frameBuffers.resize(6);
        for(int i=0;i<6;++i){

            ImageViewInfo viewInfo{};
            viewInfo.subresourceRange.baseArrayLayer=i;
            viewInfo.subresourceRange.layerCount=1;
            viewInfo.subresourceRange.levelCount=1;
            viewInfo.subresourceRange.baseMipLevel=0;
            viewInfo.subresourceRange.aspectMask=ImageAspectFlag::COLOR;
            viewInfo.format=Format::R32G32B32A32_SFLOAT;
            viewInfo.components.a=ComponentSwizzle::IDENTITY;
            viewInfo.components.r=ComponentSwizzle::IDENTITY;
            viewInfo.components.g=ComponentSwizzle::IDENTITY;
            viewInfo.components.b=ComponentSwizzle::IDENTITY;
            viewInfo.image=cubeImage;
            viewInfo.viewType=ImageViewType::e2D;

            ImageView* view;
            Context->CreateImageView(viewInfo,view);
            frameBufferAttachments.push_back(view);

            Image* depthImage;
            ImageView* depthImageView;
            DeviceMemory* depthImageMemory;

            Context->CreateImage(depthInfo,depthImage,depthImageMemory);
            depthViewInfo.image=depthImage;
            Context->CreateImageView(depthViewInfo,depthImageView);

            depth.push_back(depthImage);
            depthView.push_back(depthImageView);
            depthMemory.push_back(depthImageMemory);

            ImageView* views[2]{view,depthImageView};
            FramebufferInfo framebufferInfo{};
            framebufferInfo.extent={width,width};
            framebufferInfo.renderPass=framebuffer.renderPass;
            framebufferInfo.attachmentCount=2;
            framebufferInfo.layers=1;
            framebufferInfo.pAttachments=views;
            Context->CreateFrameBuffer(framebufferInfo,frameBuffers[i]);
        }
    }

    void ConvertCubePass::clearFrameBuffer() {
        if(!depth.empty()){
            for(int i=0;i<depth.size();++i){
                Context->DestroyImage(depth[i], depthMemory[i], depthView[i]);
            }
        }else {
            return;
        }
        depth.clear();
        for(auto& view : frameBufferAttachments){
            Context->DestroyImageView(view);
        }
        frameBufferAttachments.clear();
        for(auto&buffer:frameBuffers){
            Context->DestroyFrameBuffer(buffer);
        }
        frameBuffers.clear();
    }

    void ConvertCubePass::draw() {
        static ClearValue clearValues[2]{{0,0,0,1},{1.0f,0}};
        Viewport viewport{0,0,static_cast<float>(width),static_cast<float>(height),0,1};
        Rect2D scissor{Offset2D{0,0},Extent2D{width,height}};
        RenderPassBeginInfo beginInfo{};
        beginInfo.renderPass=framebuffer.renderPass;
        beginInfo.clearValueCount=2;
        beginInfo.pClearValues=clearValues;
        beginInfo.renderArea={0,0 ,width,width};

        auto commandBuffer = Context->BeginOnceCommandSubmit();
        static int indexes[]{0,1,2,3,4,5};

        for(int i=0;i<6;++i){
            beginInfo.framebuffer=frameBuffers[i];
            Context->BeginRenderPass(commandBuffer,beginInfo);
            Context->BindPipeline(commandBuffer,PipelineBindPoint::GRAPHICS,renderPipelines[0].pipeline);
            Context->SetViewPort(commandBuffer,viewport);
            Context->SetScissor(commandBuffer,scissor);
            Context->PushConstant(commandBuffer,renderPipelines[0].layout,ShaderStageFlag::VERTEX,0,sizeof(int),indexes+i);
            Context->BindDescriptorSets(commandBuffer,PipelineBindPoint::GRAPHICS,renderPipelines[0].layout,0,descriptorSets);
            Context->Draw(commandBuffer,36,1,0,0);
            Context->EndRenderPass(commandBuffer);
        }

        Context->EndOnceSubmit(commandBuffer);
    }
} // Z