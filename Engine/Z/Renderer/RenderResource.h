//
// Created by z on 2024-3-1.
//


#ifndef Z_RENDERER_RENDERRESOURCE_H
#define Z_RENDERER_RENDERRESOURCE_H

#include "Include/glm/glm.hpp"

#include "Z/Core/Core.h"
#include "Z/Scene/Scene.h"
#include "Z/Renderer/GraphicInterface.h"
namespace Z
{
    struct RenderResourceInitInfo
    {
        GraphicInterface *graphicContext;
    };
    // manage runtime render resource used by scene
    class Z_API RenderResource{
    public:
        struct CameraRenderData{
            Buffer* mvpMatrixBuffer;
            DeviceMemory* mvpMatrixBufferMemory;
            Buffer* modelTransBuffer;
            DeviceMemory* modelTransBufferMemory;
        };
        struct CameraTransformData{
            glm::mat4 view;
            glm::mat4 proj;
            glm::vec3 cameraPos;
        };
        struct GoData{
            glm::mat4 modelTrans;
            glm::ivec4 goIndex;
        };
        struct WorldLightRenderData{
            Buffer* lightBuffer;
            DeviceMemory* bufferMemory;
        };
        struct WorldLightData{
            glm::vec4 radiance;
            glm::vec4 position;
        };
        static void Init(RenderResourceInitInfo* info);
        static void clear() ;
        // upload data to buffer
        // copy ( (byte*)destination + desOffset, (byte*)data + inputOffset , inputSize )
        static void UpLoadData(void* data , uint64 inputSize = sizeof(CameraTransformData) , uint64 inputOffset=0 , uint64 dstOffset = 0);
        static void UpdateLightData(void*data,uint64 inputSize = sizeof(WorldLightData) , uint64 inputOffset = 0 , uint64 dstOffset =0);
        static Ref<RenderResource> GetInstance(){return instance;}
        static auto& GetCameraRenderData(){return instance->cameraRenderData;}
        static auto& GetInnerSetLayouts(){return instance->innerSetLayouts;}
        static DescriptorSet* GetInnerDescriptorSet(uint32 index){return instance->innerDescriptorSets[index];}
        static auto& GetInnerDescriptorSets(){return instance->innerDescriptorSets;}
        static void UpdateModelTransform(const glm::mat4& data,zGUID id);

        static void RegisterContext(Ref<Scene> context){
            instance->clearSceneResources();
            instance->context=context;
            instance->UpdateAllData();
        }
        static auto GetContext(){return instance->context;}
        static auto& GetGOIndexMap(){return instance->goIndexMap;}
        static auto& GetMaterialMap(){return instance->materialMeshMap;}
        static auto& GetGODescriptorMap(){return instance->goDescriptorSetMap;}

        static void UpdateAllData();

        static void Update(float deltaTime);

        static void RemoveMesh(zGUID id,const std::string& meshPath,const std::string& matPath);

        static void SetIrradianceMap(ImageView* view);

    private:
        static const uint16 maxTransMat=100;
        static const uint64 goDataBufferSize= sizeof(GoData) * maxTransMat;
        static Ref<RenderResource> instance;
        
        void InitRenderData();
        void InitSetLayout();
        void clearSceneResources(); 

        Ref<Scene> context=nullptr;

        GraphicInterface* graphicContext;

        CameraRenderData cameraRenderData;
        WorldLightRenderData lightRenderData;
        // inner set layout
        std::vector<DescriptorSetLayout*> innerSetLayouts;
        DescriptorSetLayout* boneDataLayout;
        std::vector<DescriptorSet*> innerDescriptorSets;

        std::unordered_map<zGUID,uint32> goIndexMap;
        std::unordered_map<std::string,std::unordered_map<zGUID,std::string>> materialMeshMap;
        std::unordered_map<zGUID,std::vector<DescriptorSet*>> goDescriptorSetMap;
        GoData goDataBuffer[maxTransMat];
    };

} // namespace Z


#endif //Z_RENDERER_RENDERRESOURCE_H