//
// Created by 32725 on 2023/3/18.
//

#ifndef ENGINE_TUTORIAL_TEXTURE_H
#define ENGINE_TUTORIAL_TEXTURE_H
#include <string>
#include "Z/Core/Core.h"

namespace Z {
	class Texture {
	public:
		virtual unsigned int GetWidth()const = 0;
		virtual unsigned int GetHeight()const = 0;
		virtual void Bind(unsigned int unit=0) const = 0;
		virtual void SetData(void* data, unsigned int size) = 0;
		virtual unsigned int GetRendererID() const = 0;
		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D()=default;
		static Ref<Texture2D> CreateTexture(const std::string &path);
		static Ref<Texture2D> CreateTexture(unsigned int width, unsigned int height);
	};

}


#endif //ENGINE_TUTORIAL_TEXTURE_H
