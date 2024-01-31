//
// Created by z on 24-1-30.
//

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderPipeline.h"

namespace Z {
    void RenderPipeline::Init(RenderPipelineInitInfo *initInfo) {
        Context=initInfo->graphicContext;
    }

    void RenderPipeline::draw() {
    }
} // Z