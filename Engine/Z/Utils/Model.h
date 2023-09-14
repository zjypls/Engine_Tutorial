//
// Created by z on 2023/9/14.
//

#ifndef ENGINEALL_MODEL_H
#define ENGINEALL_MODEL_H

#include <xhash>

#include "Z/Core/Core.h"
#include "Z/Renderer/VertexArray.h"
#include "Include/glm/glm/gtx/hash.hpp"

namespace Z {

	struct Vertex {
		glm::vec3 verts;
		glm::vec2 tex;
		glm::vec3 normal;
		bool operator==(const Vertex& o)const{
			return verts==o.verts&&tex==o.tex&&normal==o.normal;
		}
	};

	struct Z_API Mesh {
		Ref<VertexArray> vertexArray;

		static Ref<Mesh> LoadMesh(const std::filesystem::path &path);

	};

}


#endif //ENGINEALL_MODEL_H
