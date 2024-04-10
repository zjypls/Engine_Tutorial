//
// Created by z on 24-1-30.
//

#include "Z/Renderer/GraphicInterface.h"
#include "Z/Renderer/RenderManager.h"
#include "Z/Renderer/RenderPipeline.h"
#include "Z/Renderer/RenderResource.h"
#include "Z/Utils/ZUtils.h"
#include "Z/Renderer/Passes/UIPass.h"
#include "Z/Renderer/Passes/MainCameraPass.h"
#include "Z/Renderer/Passes/SkyboxPass.h"
#include "Z/Renderer/Passes/GeneralPass.h"
#include "Z/Renderer/Passes/ConvertCubePass.h"
#include "Z/Renderer/Passes/GenIrradiancePass.h"
#include "Z/Renderer/Passes/PickPass.h"

namespace Z {
    void RenderPipeline::Init(RenderPipelineInitInfo *initInfo) {
        Context=initInfo->graphicContext;
        uiPass=CreateRef<UIPass>();
        mainCameraPass=CreateRef<MainCameraPass>();
        auto mainCameraPassPtr=(MainCameraPass*)mainCameraPass.get();

        auto cameraPassInitInfo=MainCameraPass::MainCameraPassInitInfo{};
        cameraPassInitInfo.graphicInterface=Context;

        mainCameraPass->Init(&cameraPassInitInfo);

        auto passInfo=UIPassInitInfo{};
        passInfo.renderpass=mainCameraPass->GetRenderPass();
        uiPass->Init(&passInfo);
        mainCameraPass->PostInit();

        auto toolInitInfo = ConvertCubePassInitInfo{};
        toolInitInfo.graphicInterface=Context;
        toolInitInfo.toolShaderPath="Assets/Shaders/Tools/ConvertToCube.glsl";
        convertTool= CreateRef<ConvertCubePass>();
        convertTool->Init(&toolInitInfo);
        ((ConvertCubePass*)convertTool.get())->SetArgs("Assets/Textures/skybox/defaultSkybox/dusk/dusk_4K.hdr");
        convertTool->draw();
        skybox= ((ConvertCubePass *) convertTool.get())->GetResult();

        auto genInitInfo = ConvertCubePassInitInfo{};
        genInitInfo.graphicInterface=Context;
        genInitInfo.toolShaderPath="Assets/Shaders/Tools/genIrradiance.glsl";
        genShader = CreateRef<GenIrradiancePass>();
        genShader->Init(&genInitInfo);
        ((GenIrradiancePass*)genShader.get())->SetSourceImage(skybox);
        genShader->draw();
        //skybox=((GenIrradiancePass*)genShader.get())->GetResult();
        //to be improved
        RenderResource::SetIrradianceMap(((GenIrradiancePass*)genShader.get())->GetResult()->imageView);

        auto skyboxPassInitInfo=SkyboxPassInitInfo{};
        skyboxPassInitInfo.renderpass=mainCameraPassPtr->viewportRenderPass;
        skyboxPassInitInfo.frameBufferCount=Context->GetMaxFramesInFlight();
        skyboxPassInitInfo.framebuffer=mainCameraPassPtr->viewportFrameBuffer.data();
        skyboxPassInitInfo.width=mainCameraPassPtr->viewPortSize.x;
        skyboxPassInitInfo.height=mainCameraPassPtr->viewPortSize.y;
        skyboxPassInitInfo.graphicInterface=Context;
        skyboxPassInitInfo.skyboxView=skybox->imageView;
        skyboxPass=CreateRef<SkyboxPass>();
        skyboxPass->Init(&skyboxPassInitInfo);

        auto generalPassInitinfo=GeneralPassInitInfo{};
        generalPassInitinfo.graphicInterface=Context;
        generalPassInitinfo.renderpass=mainCameraPassPtr->viewportRenderPass;
        generalPassInitinfo.shaderPath="Assets/Shaders/PBR.glsl";
        auto pbrPass=CreateRef<GeneralPass>();
        pbrPass->Init(&generalPassInitinfo);
        generalPassMap[generalPassInitinfo.shaderPath]=pbrPass;

        auto pickPassInitInfo = PickPassInitInfo{};
        pickPassInitInfo.graphicInterface=Context;
        pickPass= CreateRef<PickPass>();
        pickPass->Init(&pickPassInitInfo);


    }

    void RenderPipeline::draw() {
        auto viewSize=((MainCameraPass*)mainCameraPass.get())->viewPortSize;
        auto width=viewSize.x;
        auto height=viewSize.y;
        Viewport viewports[1] = {{0, 0, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f}};
        Rect2D scissors[1] = {{0, 0, static_cast<uint32>(width), static_cast<uint32>(height)}};
        Rect2D renderArea = {{0, 0}, {static_cast<uint32>(width), static_cast<uint32>(height)}};
        auto currentFrameIndex = Context->GetCurrentFrameIndex();
        static RenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.renderPass = ((MainCameraPass*)mainCameraPass.get())->viewportRenderPass;
        renderPassBeginInfo.framebuffer = ((MainCameraPass*)mainCameraPass.get())->viewportFrameBuffer[currentFrameIndex].framebuffer;
        renderPassBeginInfo.renderArea = renderArea;
        renderPassBeginInfo.clearValueCount = 3;
        static ClearValue clearValues[3]={
                {{0.0f, 0.0f, 0.0f, 1.0f}},
                {{{-1}}},
                {{1.0f,0}}
        };
        Context->SetViewPort(viewports[0]);
        Context->SetScissor(scissors[0]);
        renderPassBeginInfo.pClearValues = clearValues;
        Context->BeginRenderPass(renderPassBeginInfo);

        skyboxPass->draw();

        for(auto&[path,pass]:generalPassMap){
            pass->draw();
        }
        
        Context->EndRenderPass();
        ((MainCameraPass*)mainCameraPass.get())->draw(uiPass);
    }

    void RenderPipeline::PushUIContents(ImGuiContent *content) {
        ((UIPass*)uiPass.get())->PushUIContents(content);
    }

    void RenderPipeline::clear() {
        Z_CORE_WARN("render pipeline clear !");
        Context->DestroyImage(skybox->image,skybox->memory,skybox->imageView);
        delete skybox;
        auto irMap = ((GenIrradiancePass*)genShader.get())->GetResult();
        Context->DestroyImage(irMap->image,irMap->memory,irMap->imageView);
        delete irMap;
        for(auto&[key,val]:generalPassMap){
            val->clear();
            val=nullptr;
        }
        generalPassMap.clear();
        convertTool->clear();
        genShader->clear();
        pickPass->clear();
        skyboxPass->clear();
        uiPass->clear();
        mainCameraPass->clear();
    }

    void RenderPipeline::SetViewPortSize(uint32 width, uint32 height) {
        ((MainCameraPass*)mainCameraPass.get())->SetViewPortSize(width,height);
        ((SkyboxPass*)skyboxPass.get())->SetViewportSize(width,height);
        ((PickPass*)pickPass.get())->SetViewportSize(width,height);
    }

    void *RenderPipeline::GetViewportFrameBufferDescriptor() {
        return ((MainCameraPass*)mainCameraPass.get())->GetViewportFrameBufferDescriptor();
    }

    void RenderPipeline::Resize() {
        mainCameraPass->Resize();
    }

    Ref<RenderPass> RenderPipeline::LoadPass(const std::string &path) {
        try{
            auto pass=CreateRef<GeneralPass>();
            GeneralPassInitInfo info{};
            info.shaderPath=path;
            info.graphicInterface=Context;
            info.renderpass=((MainCameraPass*)mainCameraPass.get())->viewportRenderPass;
            pass->Init(&info);
            generalPassMap[path]=pass;
            return pass;
        }catch(...){
            return nullptr;
        }
    }

    glm::uvec2 RenderPipeline::GetViewportSize() {
        return ((MainCameraPass*)mainCameraPass.get())->viewPortSize;
    }

    int RenderPipeline::PickGO(int x, int y) {
        pickPass->draw();
        return ((PickPass*)pickPass.get())->GetPickValue(x,y);
    }
} // Z