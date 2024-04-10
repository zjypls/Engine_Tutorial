//
// Created by z on 2024/4/6.
//

#ifndef ENGINEALL_CONVERTCUBEPASS_H
#define ENGINEALL_CONVERTCUBEPASS_H


#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/RenderPass.h"


namespace Z {

    struct ConvertCubePassInitInfo:public RenderPassInitInfo{
        std::string toolShaderPath;
    };

    class Z_API ConvertCubePass :public RenderPass{
    public:
        virtual void Init(RenderPassInitInfo* info)override;
        void clear()override;
        void draw()override;

        virtual void SetArgs(const std::string& path);

        Texture2D* GetResult(){
            auto res = new Texture2D;
            res->image=cubeImage;
            res->imageView=cubeView;
            res->memory=cubeMemory;
            res->width=width;
            res->height=height;
            res->path=sourcePath;
            return res;
        }

    protected:
        virtual void InitBuffer();
        virtual void InitRenderPass();
        virtual void InitFrameBuffer();
        virtual void InitPipeline();
        virtual void InitDescriptorSets();
        virtual void UpdateSourceImageSet();
        virtual void clearFrameBuffer();
        

        uint32 width=0,height=0;
        std::string shaderPath;
        std::string sourcePath;
        Image* cubeImage,*sourceImage;
        DeviceMemory* cubeMemory,*sourceImageMemory;
        ImageView* cubeView,*sourceImageView;
        std::vector<Z::Framebuffer*> frameBuffers;
        std::vector<ImageView*> frameBufferAttachments;
        std::vector<Image*> depth;
        std::vector<ImageView*> depthView;
        std::vector<DeviceMemory*> depthMemory;
        Buffer* mvBuffer;
        DeviceMemory* mvMemory;
    };

} // Z

#endif //ENGINEALL_CONVERTCUBEPASS_H
