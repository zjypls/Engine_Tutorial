//
// Created by 32725 on 2023/3/18.
//

#ifndef ENGINE_TUTORIAL_OPENGLTEXTURES_H
#define ENGINE_TUTORIAL_OPENGLTEXTURES_H

#include"Z/Renderer/Texture.h"
#include "glad/glad.h"
namespace Z {
	class OpenGLTexture2D : public Texture2D {
		unsigned int width,height;
		unsigned int ID;
		GLenum internalFormat, dataFormat;
	public:
		OpenGLTexture2D(const std::string &path);
		OpenGLTexture2D(unsigned int width, unsigned int height);
		virtual ~OpenGLTexture2D() override;
		virtual void SetData(void* data, unsigned int size) override;
		virtual bool operator==(const Texture& other) const override;
		inline virtual unsigned int GetRendererID()const override{ return ID;}

		virtual inline unsigned int GetWidth()const override{ return width;}
		virtual inline unsigned int GetHeight()const override{ return height;}
		virtual void Bind(unsigned int unit=0)const override;
	};

}


#endif //ENGINE_TUTORIAL_OPENGLTEXTURES_H
