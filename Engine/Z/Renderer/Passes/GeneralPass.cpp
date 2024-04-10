//
// Created by z on 2024/3/31.
//

#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/RenderResource.h"
#include "Z/Renderer/Passes/GeneralPass.h"

namespace Z {
    void GeneralPass::Init(RenderPassInitInfo *initInfo) {
        Context=initInfo->graphicInterface;
        auto specInfo=(GeneralPassInitInfo*)initInfo;
        materialPath=specInfo->shaderPath;
        framebuffer.renderPass=specInfo->renderpass;

        SetupPipeline();
    }

    void GeneralPass::draw(){
        auto currentIndex=Context->GetCurrentFrameIndex();
        auto& materialMap=RenderResource::GetMaterialMap();
        auto meshIt=materialMap.find(materialPath);
        if(meshIt==materialMap.end())return;
        auto& descriptorMap=RenderResource::GetGODescriptorMap();
        auto& goIndexMap=RenderResource::GetGOIndexMap();
        auto innerSets= RenderResource::GetInnerDescriptorSets();
        Context->BindPipeline(PipelineBindPoint::GRAPHICS,renderPipelines[0].pipeline);
        for(const auto&[id,meshPath]:meshIt->second){
            auto mesh=AssetsSystem::Load<MeshRes>(meshPath);
            auto index=goIndexMap[id];
            Context->PushConstant(renderPipelines[0].layout,ShaderStageFlag::VERTEX,0,sizeof(uint32),&index);
            std::vector<DescriptorSet*> sets{innerSets};
            auto descriptorIt=descriptorMap.find(id);
            if(descriptorIt!=descriptorMap.end()) {
                sets.insert(sets.end(),descriptorIt->second.begin(),descriptorIt->second.end());
            }
            Context->BindDescriptorSets(PipelineBindPoint::GRAPHICS,renderPipelines[0].layout,0,sets);
            Context->BindVertexBuffer(mesh->vertexBuffer,0,2);
            Context->BindIndexBuffer(mesh->indexBuffer);
            Context->DrawIndexed(mesh->indicesCount,1,0,0,0);
        }
    }

    void GeneralPass::SetupPipeline() {
        renderPipelines.resize(1);
        auto shaderRes= AssetsSystem::Load<ShaderRes>(materialPath);
        std::vector<DescriptorSetLayout*> layouts;
        setInfos = Context->CreateGraphicPipeline(shaderRes->source,shaderRes->stages,renderPipelines[0].pipeline,
                                       framebuffer.renderPass,layouts,renderPipelines[0].layout);
        descriptors.resize(layouts.size());
        for(int i=0;i<layouts.size();++i){
            descriptors[i].layout=layouts[i];
        }
    }

    void GeneralPass::clear(){
        Context->DestroyPipeline(renderPipelines[0].pipeline);
        Context->DestroyPipelineLayout(renderPipelines[0].layout);
        for(int i=2;i<descriptors.size();++i){
            Context->DestroyDescriptorSetLayout(descriptors[i].layout);
        }
    }

} // Z