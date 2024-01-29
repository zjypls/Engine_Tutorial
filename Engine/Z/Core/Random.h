//
// Created by 32725 on 2023/3/21.
//

#ifndef ENGINE_TUTORIAL_RANDOM_H
#define ENGINE_TUTORIAL_RANDOM_H
#include <random>
#include "Include/glm/glm/glm.hpp"
namespace Z {
	class Random {
		static std::random_device randomDevice;
		static std::mt19937 RandomEngine;
		static std::uniform_real_distribution<float> Distribution;
	public:
		inline static void Init(unsigned int seed=1){RandomEngine.seed(seed);}
		inline static float Float(){return Distribution(RandomEngine);}
		inline static float Range(float min,float max) {return min+(max-min)*Float();}
		inline static glm::vec3 RandVec3(){
			return glm::vec3{Float(),Float(),Float()};
		}
		inline static glm::vec2 RandVec2(){
			return glm::vec2{Float(),Float()};
		}

	};

}


#endif //ENGINE_TUTORIAL_RANDOM_H
