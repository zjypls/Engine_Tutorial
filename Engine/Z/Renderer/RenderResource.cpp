//
// Created by z on 2024-3-1.
//
#include "Z/Core/Log.h"
#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/RenderResource.h"
#include "Z/Renderer/GraphicInterface.h"
namespace Z
{
    Ref<RenderResource> RenderResource::instance=nullptr;
    void RenderResource::Init(RenderResourceInitInfo* info) {
        Z_CORE_ASSERT(instance == nullptr,"Already Inited !");
        instance=CreateRef<RenderResource>();
        instance->graphicContext=info->graphicContext;
        instance->maxFlightFrame=info->graphicContext->GetMaxFramesInFlight();
        instance->renderResourceData.resize(instance->maxFlightFrame);
        instance->InitRenderData();
        instance->InitSetLayout();
        auto model=AssetsSystem::Load<MeshRes>("local_model/model.fbx");
    }

    void RenderResource::clear() {
        for(auto &data:instance->renderResourceData)
        {
            instance->graphicContext->DestroyBuffer(data.mvpMatrixBuffer,data.mvpMatrixBufferMemory);
            delete data.mvpMatrixBuffer;
            delete data.mvpMatrixBufferMemory;
        }
        instance=nullptr;
    }

    void RenderResource::UpdateData(void* data,uint64 inputSize,uint64 inputOffset,uint64 desOffset) {
        const auto& renderData=instance->renderResourceData[instance->graphicContext->GetCurrentFrameIndex()];
        void* mappedData;
        instance->graphicContext->MapMemory(renderData.mvpMatrixBufferMemory,0,sizeof(InputData),mappedData);
        memcpy(((uint8*)mappedData)+desOffset,((uint8*)data)+inputOffset,inputSize);
        instance->graphicContext->UnMapMemory(renderData.mvpMatrixBufferMemory);
    }

    void RenderResource::InitRenderData() {
        BufferInfo bufferInfo{};
        bufferInfo.size=sizeof(glm::mat4)*2;
        bufferInfo.usage=BufferUsageFlag::UNIFORM_BUFFER;
        bufferInfo.properties=MemoryPropertyFlag::HOST_VISIBLE|MemoryPropertyFlag::HOST_COHERENT;
        for(auto&data:renderResourceData){
            graphicContext->CreateBuffer(bufferInfo,data.mvpMatrixBuffer,data.mvpMatrixBufferMemory);
        }
    }

    void RenderResource::InitSetLayout(){
        firstLayout=graphicContext->GetFirstDescriptorSetLayout();
        firstDescriptorSets.resize(maxFlightFrame);
        DescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.descriptorSetCount=1;
        allocateInfo.pSetLayouts=firstLayout;
        for(auto &descriptorSet:firstDescriptorSets){
            graphicContext->AllocateDescriptorSet(allocateInfo,descriptorSet);
        }
        
        WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.dstBinding=0;
        writeDescriptorSet.dstArrayElement=0;
        writeDescriptorSet.descriptorType=DescriptorType::UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount=1;

        for(int i=0;i<firstDescriptorSets.size();++i){
            DescriptorBufferInfo bufferInfo{};
            bufferInfo.offset=0;
            bufferInfo.range=sizeof(InputData);
            bufferInfo.buffer=renderResourceData[i].mvpMatrixBuffer;
            writeDescriptorSet.dstSet=firstDescriptorSets[i];
            writeDescriptorSet.pBufferInfo=&bufferInfo;
            graphicContext->WriteDescriptorSets(&writeDescriptorSet,1);
        }
        
    }

} // namespace Z
