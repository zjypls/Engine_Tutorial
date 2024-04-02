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
        auto currentFrameIndex = Context->GetCurrentFrameIndex();
        for (auto & renderPipeline : renderPipelines) {
            Context->BindPipeline(PipelineBindPoint::GRAPHICS, renderPipeline.pipeline);
            Context->BindDescriptorSets(PipelineBindPoint::GRAPHICS, renderPipeline.layout, 0,
                                        descriptorSets[currentFrameIndex]);
            Context->Draw(36, 1, 0, 0);
        }
    }

    void SkyboxPass::clear() {
        for(auto &pipeline:renderPipelines){
            Context->DestroyPipeline(pipeline.pipeline);
            Context->DestroyPipelineLayout(pipeline.layout);
            delete pipeline.pipeline;
            delete pipeline.layout;
        }
        for(int i=1;i<descriptors.size();++i){
            Context->DestroyDescriptorSetLayout(descriptors[i].layout);
            delete descriptors[i].layout;
        }
    }

    void SkyboxPass::InitPipeline() {
        renderPipelines.resize(1);
        auto skyboxShader=AssetsSystem::Load<ShaderRes>("Assets/Shaders/SkyBox.glsl");
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
        Context->CreateGraphicPipeline(skyboxShader->source, skyboxShader->stages,
                                       renderPipelines[0].pipeline, framebuffer.renderPass, layouts,
                                       renderPipelines[0].layout,&pipelineCreateInfo);
        descriptors.resize(layouts.size());
        for(int i=0;i<layouts.size();i++){
            descriptors[i].layout=layouts[i];
        }
    }

    void SkyboxPass::InitDescriptorSets(SkyboxPassInitInfo *initInfo) {
        auto renderResourceData = RenderResource::GetRenderResourceData();
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
        for (int i = 0; i < initInfo->frameBufferCount; i++) {
            descriptorSets[i][0] = RenderResource::GetFirstDescriptorSet(i);
            WriteDescriptorSet writeDescriptorSet[1]{};
            writeDescriptorSet[0].dstSet = descriptorSets[i][1];
            writeDescriptorSet[0].dstBinding = 0;
            writeDescriptorSet[0].dstArrayElement = 0;
            writeDescriptorSet[0].descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
            writeDescriptorSet[0].descriptorCount = 1;
            writeDescriptorSet[0].pImageInfo = &imageViewInfo;
            Context->WriteDescriptorSets(writeDescriptorSet, 1);
        }
    }
} // Z