//
// Created by 32725 on 2023/3/18.
//

#ifndef ENGINE_TUTORIAL_TEXTURE_H
#define ENGINE_TUTORIAL_TEXTURE_H
#include <string>
#include <vector>
#include "Z/Core/Core.h"

namespace Z {
	//Todo:Fix that load high resolution texture will crash
	class Z_API Texture {
	public:
		virtual unsigned int GetWidth()const = 0;
		virtual unsigned int GetHeight()const = 0;
		virtual void Bind(unsigned int unit=0) const = 0;
		virtual void SetData(void* data, unsigned int size) = 0;
		virtual unsigned int GetRendererID() const = 0;
		virtual bool operator==(const Texture& other) const = 0;
	};

	class Z_API Texture2D : public Texture {
	public:
		virtual ~Texture2D()=default;
		static Ref<Texture2D> CreateTexture(const std::string &path);
		static Ref<Texture2D> CreateTexture(unsigned int width, unsigned int height);
	};

	class Z_API Texture3D : public Texture{
	public:
		virtual ~Texture3D()=default;
		static Ref<Texture3D> CreateTexture(unsigned int width, unsigned int height,unsigned int depth);
	};

	class Z_API CubeMap : public Texture{
	public:
		virtual ~CubeMap()=default;
		static Ref<CubeMap> CreateTexture(const std::vector<std::string> &paths);
		static Ref<CubeMap> CreateTexture(unsigned int width, unsigned int height);

	};

}


#endif //ENGINE_TUTORIAL_TEXTURE_H
