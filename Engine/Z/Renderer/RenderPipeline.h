//
// Created by z on 24-1-30.
//

#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "Z/Core/Core.h"

namespace Z {

    class GraphicInterface;

    struct RenderPipelineInitInfo {
        GraphicInterface* graphicContext;
    };
    class Z_API RenderPipeline {
    public:

        virtual void Init(RenderPipelineInitInfo*info);

        virtual void draw();

    protected:
        GraphicInterface* Context;
    };

} // Z

#endif //RENDERPIPELINE_H
