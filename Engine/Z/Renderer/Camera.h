//
// Created by 32725 on 2023/3/27.
//

#ifndef ENGINE_TUTORIAL_CAMERA_H
#define ENGINE_TUTORIAL_CAMERA_H
#include "glm/glm.hpp"

namespace Z {
	class Camera{
	protected:
		glm::mat4 projection;
	public:
		Camera()=default;
		Camera(const glm::mat4& projection):projection(projection){}
		inline glm::mat4& operator()(){return projection;}
		inline const glm::mat4& operator()()const{return projection;}
	};

}

#endif //ENGINE_TUTORIAL_CAMERA_H
