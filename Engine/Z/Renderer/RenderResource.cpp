//
// Created by z on 2024-3-1.
//
#include "Z/Core/Log.h"
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

} // namespace Z
