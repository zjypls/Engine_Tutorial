//
// Created by 32725 on 2023/3/18.
//

#ifndef ENGINE_TUTORIAL_OPENGLTEXTURES_H
#define ENGINE_TUTORIAL_OPENGLTEXTURES_H

#include"Z/Renderer/Texture.h"
#include "glad/glad.h"
namespace Z {
	class Z_API OpenGLTexture2D final : public Texture2D {
		unsigned int width,height;
		unsigned int ID;
		GLenum internalFormat, dataFormat;
	public:
		OpenGLTexture2D(const std::string &path);
		OpenGLTexture2D(unsigned int width, unsigned int height);
		~OpenGLTexture2D() override;
		void SetData(void* data, unsigned int size) override;
		bool operator==(const Texture& other) const override{return ID==other.GetRendererID();}
		inline unsigned int GetRendererID()const override{ return ID;}

		inline unsigned int GetWidth()const override{ return width;}
		inline unsigned int GetHeight()const override{ return height;}
		void Bind(unsigned int unit=0)const override{glBindTextureUnit(unit,ID);}
	};

	class Z_API OpenGLTexture3D final : public Texture3D{
		unsigned int width,height,depth;
		unsigned int ID;
		GLenum internalFormat, dataFormat;
	public:
		OpenGLTexture3D(int width,int height,int depth);
		inline unsigned int GetWidth() const override {
			return width;
		}

		inline unsigned int GetHeight() const override {
			return height;
		}

		void Bind(unsigned int unit) const override {
			glBindTextureUnit(unit,ID);
		}

		void SetData(void *data, unsigned int size) override;

		inline unsigned int GetRendererID() const override {return ID;}

		bool operator==(const Texture &other) const override {return ID==other.GetRendererID();}
	};

	class Z_API OpenGLSkyBox final : public CubeMap{
		unsigned int width,height;
		unsigned int ID;
		GLenum internalFormat, dataFormat;
	public:

		OpenGLSkyBox(const std::vector<std::string>& paths);
		OpenGLSkyBox(unsigned int width, unsigned int height);

		inline unsigned int GetWidth() const override{return width;}

		inline unsigned int GetHeight() const override{return height;}

		void Bind(unsigned int unit) const override{glBindTextureUnit(unit,ID);}

		void SetData(void *data, unsigned int size) override;

		inline unsigned int GetRendererID() const override{return ID;}

		bool operator==(const Texture &other) const override{return ID==other.GetRendererID();}
	};

}


#endif //ENGINE_TUTORIAL_OPENGLTEXTURES_H
