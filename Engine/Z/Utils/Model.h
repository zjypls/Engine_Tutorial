//
// Created by z on 2023/9/14.
//

#ifndef ENGINEALL_MODEL_H
#define ENGINEALL_MODEL_H
#include <filesystem>

#include "Z/Core/Core.h"
#include "Z/Renderer/VertexArray.h"
#include "Include/glm/glm/gtx/hash.hpp"

namespace Z {


	struct Z_API Mesh {
		Ref<VertexArray> vertexArray;
		std::string name;

	};

}


#endif //ENGINEALL_MODEL_H
