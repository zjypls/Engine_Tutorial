//
// Created by 32725 on 2023/3/21.
//

#include "Z/Renderer/SubTex2D.h"

namespace Z {
	SubTex2D::SubTex2D(const Z::Ref<Z::Texture2D> &tex, glm::vec2 &leftDown, const glm::vec2 &rightUp)
			: texture(tex), texCords{leftDown,glm::vec2{rightUp.x,  leftDown.y},
			                         rightUp,glm::vec2{leftDown.x, rightUp.y}} {}

	Ref<SubTex2D> SubTex2D::Create(const Ref<Texture2D> &tex, const glm::vec2 &index, const glm::vec2& SpriteSize,const glm::vec2&SpriteCount) {
		glm::vec2 leftDown = index*SpriteSize/glm::vec2{tex->GetWidth(), tex->GetHeight()};
		glm::vec2 rightUp = (index+SpriteCount)*SpriteSize/glm::vec2{tex->GetWidth(), tex->GetHeight()};
		return CreateRef<SubTex2D>(tex, leftDown, rightUp);
	}

}