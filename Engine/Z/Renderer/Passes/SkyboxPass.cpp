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
        skyboxView=((SkyboxPassInitInfo*)info)->skyboxView;
        renderPipelines.resize(1);
        InitPipelineLayout();
        InitPipeline();
        InitDescriptorSets((SkyboxPassInitInfo *) info);
    }

    void SkyboxPass::draw() {
        auto currentFrameIndex = Context->GetCurrentFrameIndex();
        for (auto & renderPipeline : renderPipelines) {
            Context->BindPipeline(PipelineBindPoint::GRAPHICS, renderPipeline.pipeline);
            Context->BindDescriptorSets(PipelineBindPoint::GRAPHICS, renderPipeline.layout, 0,
                                        descriptorSets);
            Context->Draw(36, 1, 0, 0);
        }
    }

    void SkyboxPass::clear() {
        for(auto &pipeline:renderPipelines){
            Context->DestroyPipeline(pipeline.pipeline);
            Context->DestroyPipelineLayout(pipeline.layout);
        }
        Context->DestroyDescriptorSetLayout(descriptors[1].layout);
    }

    void SkyboxPass::InitPipeline() {
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
    }

    void SkyboxPass::InitDescriptorSets(SkyboxPassInitInfo *initInfo) {
        descriptorSets.resize(2);
        descriptorSets[0] = RenderResource::GetInnerDescriptorSet(0);
        DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.pSetLayouts=descriptors[1].layout;
        allocateInfo.descriptorSetCount=1;
        Context->AllocateDescriptorSet(allocateInfo,descriptorSets[1]);
        auto view=skyboxView;
        //get default skybox
        if(!view){
            std::string base_path = "Assets/Textures/skybox/defaultSkybox/";
            auto skybox = AssetsSystem::Load<Skybox>(base_path + "lake/defaultSkybox.zConf");
            view=skybox->imageView;
        }
        DescriptorImageInfo imageViewInfo{};
        imageViewInfo.imageView = view;
        imageViewInfo.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

        WriteDescriptorSet writeDescriptorSet[1]{};
        writeDescriptorSet[0].dstSet = descriptorSets[1];
        writeDescriptorSet[0].dstBinding = 0;
        writeDescriptorSet[0].dstArrayElement = 0;
        writeDescriptorSet[0].descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet[0].descriptorCount = 1;
        writeDescriptorSet[0].pImageInfo = &imageViewInfo;
        Context->WriteDescriptorSets(writeDescriptorSet, 1);

    }

    void SkyboxPass::InitPipelineLayout() {
        descriptors.resize(2);
        descriptors[0].layout=RenderResource::GetInnerSetLayouts()[0];
        DescriptorSetLayoutBinding binding{};
        binding.descriptorType=DescriptorType::COMBINED_IMAGE_SAMPLER;
        binding.binding=0;
        binding.descriptorCount=1;
        binding.stageFlags=ShaderStageFlag::FRAGMENT;
        binding.pImmutableSamplers=Context->GetDefaultSampler(SamplerType::Linear);

        DescriptorSetLayoutCreateInfo setLayoutInfo{};
        setLayoutInfo.pBindings=&binding;
        setLayoutInfo.bindingCount=1;
        Context->CreateDescriptorSetLayout(setLayoutInfo,descriptors[1].layout);
        std::vector setLayouts{descriptors[0].layout,descriptors[1].layout};

        PipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.setLayoutCount=setLayouts.size();
        layoutCreateInfo.pSetLayouts=setLayouts.data();
        layoutCreateInfo.pushConstantRangeCount=0;

        Context->CreatePipelineLayout(layoutCreateInfo,renderPipelines[0].layout);
    }
} // Z