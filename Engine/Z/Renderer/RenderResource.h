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
        static void Init(RenderResourceInitInfo* info);
        static void clear() ;
    private:
        static Ref<RenderResource> instance;
        GraphicInterface* graphicContext;
    };

} // namespace Z


#endif //Z_RENDERER_RENDERRESOURCE_H