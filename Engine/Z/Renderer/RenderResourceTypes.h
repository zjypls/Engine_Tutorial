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
        glm::vec2 uv;

        bool operator==(const Vertex& a) const{
            return a.position==position&&a.normal==normal&&a.uv==uv;
        }
    };
    struct VertexBlending{
        uint32 boneIndex[maxBoneBlending];
        float blending[maxBoneBlending];
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
        static std::array<VertexInputAttributeDescription,5> GetAttributeDescription(){
            std::array<VertexInputAttributeDescription,5> attributeDescriptions;
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
            attributeDescriptions[2].format=Format::R32G32_SFLOAT;
            attributeDescriptions[2].offset=offsetof(Vertex,uv);
            attributeDescriptions[3].binding=1;
            attributeDescriptions[3].location=3;
            attributeDescriptions[3].format=Format::R32G32B32A32_UINT;
            attributeDescriptions[3].offset=offsetof(VertexBlending,boneIndex);
            attributeDescriptions[4].binding=1;
            attributeDescriptions[4].location=4;
            attributeDescriptions[4].format=Format::R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset=offsetof(VertexBlending,blending);
            return attributeDescriptions;
        }
    };
}



namespace std {
	template<>
	struct hash<Z::Vertex> {
		size_t operator()(const Z::Vertex &v) const {
			return hash<glm::vec3>()(v.position) << 1
			       | hash<glm::vec3>()(v.normal)
			       | hash<glm::vec2>()(v.uv);
		}
	};
}


#endif //RENDERRESOURCETYPES_H
