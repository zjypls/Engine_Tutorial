//
// Created by z on 24-2-24.
//
#include "Include/stb/stb_image.h"

#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Utils/ZUtils.h"
#include "Z/Renderer/Passes/SkyboxPass.h"
#include "Z/Renderer/RenderResource.h"
namespace Z {
    void SkyboxPass::Init(RenderPassInitInfo *info) {
        framebuffer.renderPass = ((SkyboxPassInitInfo *) info)->renderpass;
        framebuffer.width = ((SkyboxPassInitInfo *) info)->width;
        framebuffer.height = ((SkyboxPassInitInfo *) info)->height;
        viewportFramebuffer = ((SkyboxPassInitInfo *) info)->framebuffer;
        viewportFrameBufferCount = ((SkyboxPassInitInfo *) info)->frameBufferCount;
        Context = info->graphicInterface;
        InitPipeline();
        InitDescriptorSets((SkyboxPassInitInfo *) info);
    }

    void SkyboxPass::draw() {
        Viewport viewports[1] = {{0, 0, static_cast<float>(framebuffer.width), static_cast<float>(framebuffer.height), 0.0f, 1.0f}};
        Rect2D scissors[1] = {{0, 0, static_cast<uint32>(framebuffer.width), static_cast<uint32>(framebuffer.height)}};
        Rect2D renderArea = {{0, 0}, {static_cast<uint32>(framebuffer.width), static_cast<uint32>(framebuffer.height)}};
        auto currentFrameIndex = Context->GetCurrentFrameIndex();
        static RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.renderPass = framebuffer.renderPass;
        renderPassBeginInfo.framebuffer = (viewportFramebuffer+currentFrameIndex)->framebuffer;
        renderPassBeginInfo.renderArea = renderArea;
        renderPassBeginInfo.clearValueCount = 3;
        static ClearValue clearValues[3]={
                {{0.0f, 0.0f, 0.0f, 1.0f}},
                {.color{.i32{-1}}},
                {.depthStencil{1.0f,0}}
        };
        renderPassBeginInfo.pClearValues = clearValues;
        Context->BeginRenderPass(renderPassBeginInfo);
        for (auto & renderPipeline : renderPipelines) {
            Context->BindPipeline(PipelineBindPoint::GRAPHICS, renderPipeline.pipeline);
            Context->SetViewPort(viewports[0]);
            Context->SetScissor(scissors[0]);
            Context->BindDescriptorSets(PipelineBindPoint::GRAPHICS, renderPipeline.layout, 0,
                                        descriptorSets[currentFrameIndex]);
            Context->Draw(36, 1, 0, 0);
        }
        Context->EndRenderPass();
    }

    void SkyboxPass::clear() {
        for(auto &pipeline:renderPipelines){
            Context->DestroyPipeline(pipeline.pipeline);
            Context->DestroyPipelineLayout(pipeline.layout);
            delete pipeline.pipeline;
            delete pipeline.layout;
        }
        for(auto &descriptor:descriptors){
            Context->DestroyDescriptorSetLayout(descriptor.layout);
            delete descriptor.layout;
        }
    }

    void SkyboxPass::InitPipeline() {
        renderPipelines.resize(1);
        auto skyboxShaderSource = Z::Utils::ReadFile("Assets/Shaders/SkyBox.glsl");
        GraphicPipelineCreateInfo pipelineCreateInfo{};
        PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
        colorBlendStateCreateInfo.logicOpEnable = false;
        colorBlendStateCreateInfo.attachmentCount = 2;
        PipelineColorBlendAttachmentState colorBlendAttachmentState[2]{};
        colorBlendAttachmentState[0].colorWriteMask = ColorComponentFlag::R | ColorComponentFlag::G |
                                                      ColorComponentFlag::B | ColorComponentFlag::A;
        colorBlendAttachmentState[0].blendEnable = false;
        colorBlendAttachmentState[1].colorWriteMask = ColorComponentFlag::R | ColorComponentFlag::G |
                                                      ColorComponentFlag::B | ColorComponentFlag::A;
        colorBlendAttachmentState[1].blendEnable = false;
        colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;
        pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

        std::vector<DescriptorSetLayout*> layouts{};
        Context->CreateGraphicPipeline(skyboxShaderSource, {ShaderStageFlag::VERTEX, ShaderStageFlag::FRAGMENT},
                                       renderPipelines[0].pipeline, framebuffer.renderPass, layouts,
                                       renderPipelines[0].layout,&pipelineCreateInfo);
        descriptors.resize(layouts.size());
        for(int i=0;i<layouts.size();i++){
            descriptors[i].layout=layouts[i];
        }
    }

    void SkyboxPass::InitDescriptorSets(SkyboxPassInitInfo *initInfo) {
        auto renderResourceData = RenderResource::GetRenderReourceData();
        descriptorSets.resize(initInfo->frameBufferCount);
        uint32 descriptorSize = descriptors.size();
        for (int i = 0; i < initInfo->frameBufferCount; i++) {
            descriptorSets[i].resize(descriptorSize);
            for (int j = 0; j < descriptorSize; j++) {
                DescriptorSetAllocateInfo allocateInfo{};
                allocateInfo.descriptorSetCount = 1;
                allocateInfo.pSetLayouts = descriptors[j].layout;
                Context->AllocateDescriptorSet(allocateInfo, descriptorSets[i][j]);
            }
        }
        //get default skybox
        std::string base_path= "Assets/Textures/skybox/defaultSkybox/";
        auto skybox=AssetsSystem::Load<Skybox>(base_path+"lake/defaultSkybox.zConf");
        DescriptorImageInfo imageViewInfo{};
        imageViewInfo.imageView = skybox->imageView;
        imageViewInfo.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
        DescriptorBufferInfo bufferInfo{};
        bufferInfo.offset=0;
        bufferInfo.range=sizeof(RenderResource::InputData);
        for (int i = 0; i < initInfo->frameBufferCount; i++) {
            WriteDescriptorSet writeDescriptorSet[2]{};
            writeDescriptorSet[0].dstSet = descriptorSets[i][0];
            writeDescriptorSet[0].dstBinding = 0;
            writeDescriptorSet[0].dstArrayElement = 0;
            writeDescriptorSet[0].descriptorType = DescriptorType::UNIFORM_BUFFER;
            writeDescriptorSet[0].descriptorCount = 1;
            bufferInfo.buffer = renderResourceData[i].mvpMatrixBuffer;
            writeDescriptorSet[0].pBufferInfo = &bufferInfo;
            writeDescriptorSet[1].dstSet = descriptorSets[i][1];
            writeDescriptorSet[1].dstBinding = 0;
            writeDescriptorSet[1].dstArrayElement = 0;
            writeDescriptorSet[1].descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
            writeDescriptorSet[1].descriptorCount = 1;
            writeDescriptorSet[1].pImageInfo = &imageViewInfo;
            Context->WriteDescriptorSets(writeDescriptorSet, 2);
        }
    }
} // Z