//
// Created by 32725 on 2023/3/21.
//

#ifndef ENGINE_TUTORIAL_PARTICLE_H
#define ENGINE_TUTORIAL_PARTICLE_H
#include <vector>
#include <list>
#include "Include/glm/glm/glm.hpp"
/*namespace Z {
	struct ParticleProp{
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 velocityVariation;
		glm::vec4 color;
		float size;
		float sizeVariation;
		float lifeTime = 1.0f;
	};
	class Particle {
		static std::vector<ParticleProp> ParticlePool;
		static unsigned int ParticlePoolIndex,MaxParticles;
	public:
		static void AddToPool(const ParticleProp& particleProp);
		static void OnUpdate(float deltaTime);
		static void OnRender();
		static void Init(unsigned int maxParticles=1000);
		static unsigned int GetCurrentNum(){return ParticlePoolIndex;};
	};

}*/
//Todo : use list instead of vector
namespace Z {
	struct ParticleProp{
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 velocityVariation;
		glm::vec4 color;
		float size;
		float sizeVariation;
		float lifeTime = 1.0f;
	};
	class Particle {
		static std::list<ParticleProp> ParticlePool;
		static unsigned int ParticlePoolIndex,MaxParticles;
	public:
		static void AddToPool(const ParticleProp&& particleProp);
		static void OnUpdate(float deltaTime);
		static void OnRender();
		static void Init(unsigned int maxParticles=1000);
		static unsigned int GetCurrentNum(){return ParticlePoolIndex;};
	};

}



#endif //ENGINE_TUTORIAL_PARTICLE_H
