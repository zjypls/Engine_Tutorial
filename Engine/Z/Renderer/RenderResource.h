//
// Created by z on 2024-3-1.
//


#ifndef Z_RENDERER_RENDERRESOURCE_H
#define Z_RENDERER_RENDERRESOURCE_H

#include "Include/glm/glm.hpp"

#include "Z/Core/Core.h"
#include "Z/Renderer/GraphicInterface.h"
namespace Z
{
    struct RenderResourceInitInfo
    {
        GraphicInterface *graphicContext;
    };
    class Z_API RenderResource
    {
    public:
        struct RenderResourceData{
            Buffer* mvpMatrixBuffer;
            DeviceMemory* mvpMatrixBufferMemory;
        };
        struct InputData{
            glm::mat4 view;
            glm::mat4 proj;
        };
        static void Init(RenderResourceInitInfo* info);
        static void clear() ;
        // upload data to buffer
        // copy ( (byte*)destination + desOffset, (byte*)data + inputOffset , inputSize )
        static void UpdateData(void* data , uint64 inputSize = sizeof(InputData) , uint64 inputOffset=0 , uint64 desOffset = 0);
        static RenderResource* GetInstance(){return instance.get();}
        static RenderResourceData* GetCurrentRenderResourceData(){return &instance->renderResourceData[instance->graphicContext->GetCurrentFrameIndex()];}
        static const auto& GetRenderReourceData(){return instance->renderResourceData;}

    private:
        static Ref<RenderResource> instance;
        void InitRenderData();
        uint32 maxFlightFrame=0;
        GraphicInterface* graphicContext;
        std::vector<RenderResourceData> renderResourceData;
    };

} // namespace Z


#endif //Z_RENDERER_RENDERRESOURCE_H