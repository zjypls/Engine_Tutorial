//
// Created by z on 24-1-15.
//

#ifndef ENGINEALL_GRAPHICINTERFACE_H
#define ENGINEALL_GRAPHICINTERFACE_H

#include "Z/Renderer/RenderResource.h"

namespace Z {

    struct GraphicSpec {

    };

    class GraphicInterface {
    public:
        virtual void Init(const GraphicSpec &initInfo) = 0;

        virtual void prepare() = 0;

        virtual void CreateImage(const ImageInfo &info, Image* &image, DeviceMemory* &memory) = 0;

        virtual void CreateBuffer(const BufferInfo &info, Buffer* &buffer, DeviceMemory* &memory) = 0;
    };

} // Z

#endif //ENGINEALL_GRAPHICINTERFACE_H
