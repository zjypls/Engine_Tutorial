//
// Created by z on 24-2-2.
//

#ifndef RENDERRESOURCETYPES_H
#define RENDERRESOURCETYPES_H
#include <array>
#include "Include/glm/glm.hpp"
#include "Include/glm/gtx/hash.hpp"

#include "Z/Renderer/RenderInterface.h"

namespace Z{
    constexpr uint32 maxBoneBlending=4;
    struct Vertex{
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec2 uv;

        bool operator==(const Vertex& a) const{
            return a.position==position&&a.normal==normal&&a.uv==uv&&a.tangent==tangent;
        }
    };
    struct VertexBlending{
        int32 boneIndex[maxBoneBlending]{-1,-1,-1,-1};
        float blending[maxBoneBlending]{0,0,0,0};
        bool operator==(const VertexBlending& a) const{
            for(int i=0;i<maxBoneBlending;i++){
                if(a.boneIndex[i]!=boneIndex[i]||a.blending[i]!=blending[i]){
                    return false;
                }
            }
            return true;
        }
    };
    struct BoneData{
        glm::mat4 offsetTransform;
    };
    struct BoneDes{
        BoneData data;
        std::string name;
    };
    struct MeshDescription{
        static std::array<VertexInputBindingDescription,2> GetBindingDescription(){
            std::array<VertexInputBindingDescription,2> bindingDescriptions;
            bindingDescriptions[0].binding=0;
            bindingDescriptions[0].stride=sizeof(Vertex);
            bindingDescriptions[0].inputRate=VertexInputRate::VERTEX;
            bindingDescriptions[1].binding=1;
            bindingDescriptions[1].stride=sizeof(VertexBlending);
            bindingDescriptions[1].inputRate=VertexInputRate::VERTEX;
            return bindingDescriptions;
        }
        static std::array<VertexInputAttributeDescription,6> GetAttributeDescription(){
            std::array<VertexInputAttributeDescription,6> attributeDescriptions;
            attributeDescriptions[0].binding=0;
            attributeDescriptions[0].location=0;
            attributeDescriptions[0].format=Format::R32G32B32_SFLOAT;
            attributeDescriptions[0].offset=offsetof(Vertex,position);
            attributeDescriptions[1].binding=0;
            attributeDescriptions[1].location=1;
            attributeDescriptions[1].format=Format::R32G32B32_SFLOAT;
            attributeDescriptions[1].offset=offsetof(Vertex,normal);
            attributeDescriptions[2].binding=0;
            attributeDescriptions[2].location=2;
            attributeDescriptions[2].format=Format::R32G32B32_SFLOAT;
            attributeDescriptions[2].offset=offsetof(Vertex,tangent);
            attributeDescriptions[3].binding=0;
            attributeDescriptions[3].location=3;
            attributeDescriptions[3].format=Format::R32G32_SFLOAT;
            attributeDescriptions[3].offset=offsetof(Vertex,uv);
            attributeDescriptions[4].binding=1;
            attributeDescriptions[4].location=4;
            attributeDescriptions[4].format=Format::R32G32B32A32_SINT;
            attributeDescriptions[4].offset=offsetof(VertexBlending,boneIndex);
            attributeDescriptions[5].binding=1;
            attributeDescriptions[5].location=5;
            attributeDescriptions[5].format=Format::R32G32B32A32_SFLOAT;
            attributeDescriptions[5].offset=offsetof(VertexBlending,blending);
            return attributeDescriptions;
        }
    };
}



namespace std {
	template<>
	struct hash<Z::Vertex> {
		Z::uint64 operator()(const Z::Vertex &v) const {
			return hash<glm::vec3>()(v.position) << 1
			       | hash<glm::vec3>()(v.normal)
                   | hash<glm::vec3>()(v.tangent)
			       | hash<glm::vec2>()(v.uv);
		}
	};
    template<>
    struct hash<Z::VertexBlending> {
        Z::uint64 operator()(const Z::VertexBlending &v) const {
            Z::uint64 result=0;
            for(int i=0;i<Z::maxBoneBlending;i++){
                result|=hash<Z::uint32>()(v.boneIndex[i])<<i;
                result|=hash<float>()(v.blending[i])<<i;
            }
            return result;
        }
    };
}


#endif //RENDERRESOURCETYPES_H
