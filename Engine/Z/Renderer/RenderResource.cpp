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
    }

    void RenderResource::clear() {
        instance=nullptr;
    }

} // namespace Z
