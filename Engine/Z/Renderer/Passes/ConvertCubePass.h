//
// Created by z on 2024/4/6.
//

#ifndef ENGINEALL_CONVERTCUBEPASS_H
#define ENGINEALL_CONVERTCUBEPASS_H


#include "Z/Core/AssetsSystem.h"
#include "Z/Renderer/RenderPass.h"


namespace Z {

    struct ConvertCubePassInitInfo:public RenderPassInitInfo{
    };

    class ConvertCubePass :public RenderPass{
    public:
        void Init(RenderPassInitInfo* info)override;
        void clear()override;
        void draw()override;

        void SetArgs(const std::string& path);

        Texture2D* GetSkybox(){
            auto res = new Texture2D;
            res->image=cubeImage;
            res->imageView=cubeView;
            res->memory=cubeMemory;
            return res;
        }

    private:
        void InitBuffer();
        void InitRenderPass();
        void InitFrameBuffer();
        void InitPipeline();
        void InitDescriptorSets();

        void UpdateSourceImageSet();

        void clearFrameBuffer();
        

        uint32 width=0,height=0;
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
