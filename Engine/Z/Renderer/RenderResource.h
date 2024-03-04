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
        struct InputData{
        };
        static void Init(RenderResourceInitInfo* info);
        static void clear() ;
        // upload data to buffer
        // copy ( destination + desOffset, data + inputOffset , inputSize)
        static void UpdateData(InputData* data , uint64 inputOffset=0 , uint64 inputSize = sizeof(InputData) , uint64 desOffset = 0);
        static RenderResource* GetInstance(){return instance.get();}
    private:
        static Ref<RenderResource> instance;
        GraphicInterface* graphicContext;
    };

} // namespace Z


#endif //Z_RENDERER_RENDERRESOURCE_H