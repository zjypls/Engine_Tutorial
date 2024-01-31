//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_CAMERA_H
#define ENGINE_TUTORIAL_CAMERA_H
#include "Z/Core/Core.h"
#include "Include/glm/glm/glm.hpp"

namespace Z {
	class Camera{
	protected:
		glm::mat4 projection;
	public:
		Camera()=default;
		Camera(const glm::mat4& projection):projection(projection){}
		glm::mat4& operator()(){return projection;}
		const glm::mat4& operator()()const{return projection;}
	};

}

#endif //ENGINE_TUTORIAL_CAMERA_H
