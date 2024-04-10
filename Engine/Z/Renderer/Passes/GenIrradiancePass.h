//
// Created by z on 2024/4/8.
//

#ifndef ENGINEALL_GENIRRADIANCEPASS_H
#define ENGINEALL_GENIRRADIANCEPASS_H

#include "Z/Renderer/Passes/ConvertCubePass.h"

namespace Z {

    struct GenIrradiancePassInitInfo : public ConvertCubePassInitInfo{

    };

    class Z_API GenIrradiancePass final : public ConvertCubePass{
    public:

        void SetSourceImage(Texture2D* source){
            if(source->path.empty() || source->path==sourcePath)return;
            sourcePath=source->path;
            sourceImage=source->image;
            sourceImageMemory=source->memory;
            sourceImageView=source->imageView;
            UpdateSourceImageSet();
            if(width==source->width)return;
            clearFrameBuffer();
            width=source->width;
            height=source->height;
            InitFrameBuffer();
        }

        [[warning("using SetSourceImage instead")]]
        void SetArgs(const std::string& path)override{
        }

    protected:

    };

} // Z

#endif //ENGINEALL_GENIRRADIANCEPASS_H
