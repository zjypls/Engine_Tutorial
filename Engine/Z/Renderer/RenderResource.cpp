//
// Created by z on 2024-3-1.
//
#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Core/Application.h"
#include "Z/Scene/Components.h"
#include "Z/Scene/Entity.hpp"
#include "Z/Renderer/RenderResource.h"
#include "Z/Renderer/RenderManager.h"
#include "Z/Renderer/GraphicInterface.h"
namespace Z
{
    Ref<RenderResource> RenderResource::instance=nullptr;
    void RenderResource::Init(RenderResourceInitInfo* info) {
        Z_CORE_ASSERT(instance == nullptr,"Already Inited !");
        instance=CreateRef<RenderResource>();
        instance->graphicContext=info->graphicContext;
        instance->InitRenderData();
        instance->InitSetLayout();
    }

    void RenderResource::clear() {
        auto& context=instance->graphicContext;
        auto& data=instance->cameraRenderData;
        context->DestroyBuffer(data.mvpMatrixBuffer,data.mvpMatrixBufferMemory);
        context->DestroyBuffer(data.modelTransBuffer,data.modelTransBufferMemory);
        context->DestroyBuffer(instance->lightRenderData.lightBuffer,instance->lightRenderData.bufferMemory);
        delete data.mvpMatrixBuffer;
        delete data.modelTransBuffer;
        delete data.mvpMatrixBufferMemory;
        delete data.modelTransBufferMemory;
        delete instance->lightRenderData.lightBuffer;
        delete instance->lightRenderData.bufferMemory;
        instance->clearSceneResources();
        context->DestroyDescriptorSetLayout(instance->boneDataLayout);

        instance=nullptr;
    }

    void RenderResource::UpLoadData(void* data,uint64 inputSize,uint64 inputOffset,uint64 desOffset) {
        const auto& renderData=instance->cameraRenderData;
        void* mappedData;
        instance->graphicContext->MapMemory(renderData.mvpMatrixBufferMemory, 0, sizeof(CameraTransformData), mappedData);
        memcpy(((uint8*)mappedData)+desOffset,((uint8*)data)+inputOffset,inputSize);
        instance->graphicContext->UnMapMemory(renderData.mvpMatrixBufferMemory);
    }

    void RenderResource::InitRenderData() {
        BufferInfo bufferInfo{};
        bufferInfo.size=sizeof(CameraTransformData);
        bufferInfo.usage=BufferUsageFlag::UNIFORM_BUFFER;
        bufferInfo.properties=MemoryPropertyFlag::HOST_VISIBLE|MemoryPropertyFlag::HOST_COHERENT;
        BufferInfo modelBufferInfo{};
        modelBufferInfo.size=goDataBufferSize;
        modelBufferInfo.usage=BufferUsageFlag::STORAGE_BUFFER;
        modelBufferInfo.properties=MemoryPropertyFlag::HOST_VISIBLE|MemoryPropertyFlag::HOST_COHERENT;
        graphicContext->CreateBuffer(bufferInfo,cameraRenderData.mvpMatrixBuffer,cameraRenderData.mvpMatrixBufferMemory);
        graphicContext->CreateBuffer(modelBufferInfo,cameraRenderData.modelTransBuffer,cameraRenderData.modelTransBufferMemory);

        BufferInfo lightDataBufferInfo{};
        lightDataBufferInfo.size=sizeof(WorldLightData);
        lightDataBufferInfo.properties=MemoryPropertyFlag::HOST_COHERENT|MemoryPropertyFlag::HOST_VISIBLE;
        lightDataBufferInfo.usage=BufferUsageFlag::UNIFORM_BUFFER;
        graphicContext->CreateBuffer(lightDataBufferInfo,lightRenderData.lightBuffer,lightRenderData.bufferMemory);
    }

    void RenderResource::InitSetLayout(){
        constexpr auto cameraDataSet=RenderManager::CameraDataTransformSet;
        constexpr auto lightDataSet=RenderManager::WorldLightDataSet;
        innerSetLayouts= graphicContext->GetInnerDescriptorSetLayout();
        innerDescriptorSets.resize(lightDataSet+1);

        DescriptorSetLayoutBinding binding{};
        binding.binding=0;
        binding.descriptorCount=1;
        binding.descriptorType=DescriptorType::STORAGE_BUFFER;
        binding.stageFlags=ShaderStageFlag::VERTEX;

        DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount=1;
        layoutInfo.pBindings=&binding;
        graphicContext->CreateDescriptorSetLayout(layoutInfo,boneDataLayout);

        DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.descriptorSetCount=1;
        allocateInfo.pSetLayouts=innerSetLayouts[cameraDataSet];
        graphicContext->AllocateDescriptorSet(allocateInfo,innerDescriptorSets[cameraDataSet]);

        allocateInfo.descriptorSetCount=1;
        allocateInfo.pSetLayouts=innerSetLayouts[lightDataSet];
        graphicContext->AllocateDescriptorSet(allocateInfo,innerDescriptorSets[lightDataSet]);


        DescriptorBufferInfo bufferInfo[2]{};
        bufferInfo[0].offset=0;
        bufferInfo[0].range=sizeof(CameraTransformData);
        bufferInfo[0].buffer=cameraRenderData.mvpMatrixBuffer;
        bufferInfo[1].offset=0;
        bufferInfo[1].range=sizeof(GoData)*maxTransMat;
        bufferInfo[1].buffer=cameraRenderData.modelTransBuffer;

        WriteDescriptorSet writeDescriptorSet[2]{};
        writeDescriptorSet[0].dstBinding=0;
        writeDescriptorSet[0].dstArrayElement=0;
        writeDescriptorSet[0].descriptorType=DescriptorType::UNIFORM_BUFFER;
        writeDescriptorSet[0].descriptorCount=1;
        writeDescriptorSet[0].dstSet=innerDescriptorSets[cameraDataSet];
        writeDescriptorSet[0].pBufferInfo=bufferInfo;
        writeDescriptorSet[1].dstBinding=1;
        writeDescriptorSet[1].dstArrayElement=0;
        writeDescriptorSet[1].descriptorType=DescriptorType::STORAGE_BUFFER;
        writeDescriptorSet[1].descriptorCount=1;
        writeDescriptorSet[1].dstSet=innerDescriptorSets[cameraDataSet];
        writeDescriptorSet[1].pBufferInfo=bufferInfo+1;
        graphicContext->WriteDescriptorSets(writeDescriptorSet,2);

        DescriptorBufferInfo lightBufferInfo{};
        lightBufferInfo.range=sizeof(WorldLightData);
        lightBufferInfo.offset=0;
        lightBufferInfo.buffer=lightRenderData.lightBuffer;

        WriteDescriptorSet lightSetWrite{};
        lightSetWrite.descriptorCount=1;
        lightSetWrite.descriptorType=DescriptorType::UNIFORM_BUFFER;
        lightSetWrite.dstBinding=0;
        lightSetWrite.dstArrayElement=0;
        lightSetWrite.pBufferInfo=&lightBufferInfo;
        lightSetWrite.dstSet=innerDescriptorSets[lightDataSet];
        graphicContext->WriteDescriptorSets(&lightSetWrite,1);
    }


    void RenderResource::Update(float deltaTime){

    }

    void RenderResource::clearSceneResources() {

    }

} // namespace Z
