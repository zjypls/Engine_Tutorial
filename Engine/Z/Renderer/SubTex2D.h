//
// Created by 32725 on 2023/3/21.
//

#ifndef ENGINE_TUTORIAL_SUBTEX2D_H
#define ENGINE_TUTORIAL_SUBTEX2D_H

#include "Include/glm/glm/glm.hpp"

#include "Z/Renderer/Texture.h"

namespace Z {
	class SubTex2D {
		const Ref<Texture2D> texture;
		glm::vec2 texCords[4];
	public:
		SubTex2D(const Ref<Texture2D> &tex, glm::vec2 &leftDown, const glm::vec2 &rightUp);

		static Ref<SubTex2D> Create(const Ref<Texture2D> &tex, const glm::vec2 &index, const glm::vec2 &SpriteSize,const glm::vec2 &SpriteCount=glm::vec2{1.f,1.f});

		inline const glm::vec2 *GetCords() const { return texCords; }

		inline const Ref<Texture2D> &GetTex() const { return texture; }
	};

}

#endif //ENGINE_TUTORIAL_SUBTEX2D_H
