//
// Created by 32725 on 2023/3/21.
//

#include "Particle.h"
#include"Renderer2D.h"
#include "Z/Core/Random.h"

/*
namespace Z{

	std::vector<ParticleProp> Particle::ParticlePool;
	unsigned int Particle::ParticlePoolIndex,Particle::MaxParticles;

	void Particle::OnUpdate(float deltaTime) {
		for(int i=0;i<ParticlePoolIndex;i++) {
			ParticlePool[i].position += ParticlePool[i].velocity * deltaTime;
			ParticlePool[i].lifeTime -= deltaTime;
			ParticlePool[i].velocity += ParticlePool[i].velocityVariation * deltaTime*Random::Range(-1.f,1.f);
			ParticlePool[i].size += ParticlePool[i].sizeVariation * deltaTime;
			//ParticlePool[i].color.a -= deltaTime*sqrtf(Random::Float());
		}
		for(int i=0;i<ParticlePoolIndex;i++) {
			if(ParticlePool[i].lifeTime <= 0.0f) {
				int count=1;
				for(int j=i+1;j<ParticlePoolIndex;j++) {
					if(ParticlePool[j].lifeTime <= 0.0f) {
						count++;
					} else{
						ParticlePool[i] = ParticlePool[j];
					}
				}
				ParticlePoolIndex -= count;
			}
		}
	}

	void Particle::OnRender() {
		for(int i=0;i<ParticlePoolIndex;i++) {
			Z::Renderer2D::DrawQuad(ParticlePool[i].position, glm::vec2{ParticlePool[i].size,ParticlePool[i].size}, ParticlePool[i].color);
		}
	}

	void Particle::Init(unsigned int maxParticles) {
		MaxParticles = maxParticles;
		ParticlePool.resize(MaxParticles);
		ParticlePoolIndex = 0;
	}

	void Particle::AddToPool(const ParticleProp& particleProp) {
		if(ParticlePoolIndex >= MaxParticles)
			return;
		ParticlePool[ParticlePoolIndex] = particleProp;
		ParticlePoolIndex = (ParticlePoolIndex + 1) % MaxParticles;
	}
}
*/

namespace Z{

	std::list<ParticleProp> Particle::ParticlePool;
	unsigned int Particle::ParticlePoolIndex,Particle::MaxParticles;

	void Particle::AddToPool(const ParticleProp &&particleProp) {
		ParticlePool.push_back(particleProp);
		++ParticlePoolIndex;
	}

	void Particle::OnUpdate(float deltaTime) {
		for(auto it=ParticlePool.begin();it!=ParticlePool.end();it++) {
			it->position += it->velocity * deltaTime;
			it->lifeTime -= deltaTime;
			it->velocity += it->velocityVariation * deltaTime*Z::Random::Range(-1.f,1.f);
			it->size += it->sizeVariation * deltaTime;
			//it->color.a -= deltaTime*sqrtf(Random::Float());
		}
		for(auto it=ParticlePool.begin();it!=ParticlePool.end();) {
			if(it->lifeTime <= 0.0f) {
				it = ParticlePool.erase(it);
				--ParticlePoolIndex;
			} else{
				it++;
			}
		}
	}

	void Particle::OnRender() {
		if(ParticlePool.empty())
			int a=0;
		for(auto it=ParticlePool.begin();it!=ParticlePool.end();++it) {
			//Z::Renderer2D::DrawQuad(it->position, {it->size,it->size}, it->color);
			Z::Renderer2D::DrawQuadRotated(it->position, glm::vec2{it->size,it->size},Z::Random::Float(), it->color);
		}
	}

	void Particle::Init(unsigned int maxParticles) {
		MaxParticles = maxParticles;
		ParticlePoolIndex = 0;
	}
}