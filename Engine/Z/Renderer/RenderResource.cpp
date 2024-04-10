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

    void RenderResource::UpdateModelTransform(const glm::mat4 &data, zGUID id) {
        auto it=instance->goIndexMap.find(id);
        if(it==instance->goIndexMap.end())return;
        instance->goDataBuffer[it->second].modelTrans=data;
    }

    void RenderResource::UpdateAllData() {
        int index=0;
        instance->context->GetComponentView<MeshFilterComponent>().each([&](auto id,auto& meshFilter){
            Entity entity(id,instance->context.get());
            if(!entity.HasComponent<MeshRendererComponent>())return;
            const auto& uidComponent=entity.GetComponent<IDComponent>();
            const auto& meshRenderer= entity.GetComponent<MeshRendererComponent>();
            auto pass = RenderManager::GetPass(meshRenderer.materialPath);
            Z_CORE_ASSERT(pass,"Unknown error when loading render pass !");

            instance->materialMeshMap[meshRenderer.materialPath][uidComponent.ID]=meshFilter.meshPath;
            instance->goIndexMap[uidComponent.ID]=index;
            auto transform=entity.GetComponent<TransformComponent>();
            instance->goDataBuffer[index].modelTrans=transform.GetTransform();
            instance->goDataBuffer[index].goIndex.x=(uint32)id;
            ++index;
            auto meshRes=AssetsSystem::Load<MeshRes>(meshFilter.meshPath);
            auto destVec=instance->goDescriptorSetMap.find(uidComponent.ID);
            int offset=1;
            if(meshRes->animator && (!(destVec!=instance->goDescriptorSetMap.end() && !destVec->second.empty()) ||
                destVec->second[RenderManager::BoneNodeTransformSet-RenderManager::WorldLightDataSet-1]==nullptr)){
                DescriptorSet* set;
                DescriptorSetAllocateInfo allocateInfo{};
                allocateInfo.descriptorPool=nullptr;
                allocateInfo.descriptorSetCount=1;
                allocateInfo.pSetLayouts=instance->boneDataLayout;
                instance->graphicContext->AllocateDescriptorSet(allocateInfo,set);
                auto bufferSize=meshRes->animator->GetMatrices().size()*sizeof(glm::mat4);
                DescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer=meshRes->boneBuffer;
                bufferInfo.offset=0;
                bufferInfo.range=bufferSize;

                WriteDescriptorSet writeSet{};
                writeSet.descriptorCount=1;
                writeSet.dstBinding=0;
                writeSet.dstArrayElement=0;
                writeSet.descriptorType=DescriptorType::STORAGE_BUFFER;
                writeSet.dstSet=set;
                writeSet.pBufferInfo=&bufferInfo;

                instance->graphicContext->WriteDescriptorSets(&writeSet,1);
                if(destVec==instance->goDescriptorSetMap.end()||destVec->second.empty())
                    instance->goDescriptorSetMap[uidComponent.ID].push_back(set);
                else
                    destVec->second[RenderManager::BoneNodeTransformSet-RenderManager::WorldLightDataSet-1]=set;
                offset=0;
            }

            destVec=instance->goDescriptorSetMap.find(uidComponent.ID);
            if(destVec!=instance->goDescriptorSetMap.end()&&destVec->second.size()>=RenderManager::MaterialTextureSet-1-offset)return;

            DescriptorSetAllocateInfo allocateInfo{};
            allocateInfo.pSetLayouts=pass->GetSetLayout(RenderManager::MaterialTextureSet-offset);
            allocateInfo.descriptorPool= nullptr;
            allocateInfo.descriptorSetCount=1;

            DescriptorSet* set;
            instance->graphicContext->AllocateDescriptorSet(allocateInfo,set);
            const auto sampler=instance->graphicContext->GetDefaultSampler(SamplerType::Linear);

            auto& setInfo = pass->GetPassSetInfo()[RenderManager::MaterialTextureSet-offset];

            for(int i=0;i<setInfo.bindings.size();++i){
                DescriptorImageInfo imageInfo{};
                imageInfo.imageView=AssetsSystem::GetDefaultTexture(setInfo.bindings[i].name)->imageView;
                imageInfo.imageLayout=ImageLayout::SHADER_READ_ONLY_OPTIMAL;
                imageInfo.sampler=sampler;
                WriteDescriptorSet writeDescriptorSet{};
                writeDescriptorSet.dstBinding=i;
                writeDescriptorSet.dstSet=set;
                writeDescriptorSet.descriptorType=DescriptorType::COMBINED_IMAGE_SAMPLER;
                writeDescriptorSet.descriptorCount=1;
                writeDescriptorSet.pImageInfo=&imageInfo;
                writeDescriptorSet.pBufferInfo= nullptr;
                writeDescriptorSet.pTexelBufferView= nullptr;
                writeDescriptorSet.dstArrayElement=0;
                instance->graphicContext->WriteDescriptorSets(&writeDescriptorSet,1);
            }
            instance->goDescriptorSetMap[uidComponent.ID].push_back(set);
        });
    }

    void RenderResource::Update(float deltaTime){

        void* data= nullptr;
        instance->graphicContext->MapMemory(instance->cameraRenderData.modelTransBufferMemory, 0, goDataBufferSize, data);
        Z_CORE_ASSERT(data,"failed to map memory ! ");
        std::memcpy(data, instance->goDataBuffer, goDataBufferSize);
        instance->graphicContext->UnMapMemory(instance->cameraRenderData.modelTransBufferMemory);

        for(auto&[key,val]:instance->materialMeshMap){
            for(auto&[id,path]:val){
                auto mesh=AssetsSystem::Load<MeshRes>(path);
                if(mesh->animator){
                    auto bufferSize=mesh->animator->GetMatrices().size()*sizeof(glm::mat4);
                    void* data= nullptr;
                    instance->graphicContext->MapMemory(mesh->boneMemory,0,bufferSize,data);
                    Z_CORE_ASSERT(data,"failed to map memory ! ");
                    std::memcpy(data,mesh->animator->GetMatrices().data(),bufferSize);
                    instance->graphicContext->UnMapMemory(mesh->boneMemory);
                }
            }
        }
    }

    void RenderResource::clearSceneResources() {
        graphicContext->DeviceWaiteIdle();
        for(auto&[key,val]:goDescriptorSetMap){
            for(auto&set:val){
                graphicContext->FreeDescriptorSet(set);
            }
            val.clear();
        }
        goDescriptorSetMap.clear();
        for(auto&[key,val]:materialMeshMap){
            val.clear();
        }
        materialMeshMap.clear();
    }

    void RenderResource::UpdateLightData(void *data, uint64 inputSize, uint64 inputOffset, uint64 dstOffset) {
        Z_CORE_ASSERT(inputSize>0,"illegal data size !");
        void *dataptr= nullptr;
        instance->graphicContext->MapMemory(instance->lightRenderData.bufferMemory,0,sizeof(WorldLightData),dataptr);
        Z_CORE_ASSERT(dataptr,"failed to map memory !");
        std::memcpy(((uint8*)dataptr)+dstOffset,((uint8*)data)+inputOffset,inputSize);
        instance->graphicContext->UnMapMemory(instance->lightRenderData.bufferMemory);
    }

    void RenderResource::RemoveMesh(zGUID id, const std::string &meshPath,const std::string& matPath) {
        auto &boneSet = instance->goDescriptorSetMap.at(id)[RenderManager::BoneNodeTransformSet -
                                                            RenderManager::WorldLightDataSet - 1];
        instance->graphicContext->FreeDescriptorSet(boneSet);
        delete boneSet;
        boneSet = nullptr;
        instance->materialMeshMap[matPath].erase(id);
    }

    void RenderResource::SetIrradianceMap(ImageView *view) {

        DescriptorImageInfo imageInfo{};
        imageInfo.imageView=view;
        imageInfo.sampler=instance->graphicContext->GetDefaultSampler(SamplerType::Linear);
        imageInfo.imageLayout=ImageLayout::SHADER_READ_ONLY_OPTIMAL;

        WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.descriptorCount=1;
        writeDescriptorSet.dstSet=instance->innerDescriptorSets[1];
        writeDescriptorSet.descriptorType=DescriptorType::COMBINED_IMAGE_SAMPLER;
        writeDescriptorSet.dstArrayElement=0;
        writeDescriptorSet.pImageInfo=&imageInfo;
        writeDescriptorSet.dstBinding=1;

        instance->graphicContext->WriteDescriptorSets(&writeDescriptorSet,1);
    }

} // namespace Z
