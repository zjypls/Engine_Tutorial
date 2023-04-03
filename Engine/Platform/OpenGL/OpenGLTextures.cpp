//
// Created by 32725 on 2023/3/18.
//
#define STB_IMAGE_IMPLEMENTATION
#include "Z/Core/Log.h"
#include "stb/stb_image.h"

#include "OpenGLTextures.h"
#include "glad/glad.h"
namespace Z {
	OpenGLTexture2D::OpenGLTexture2D(const std::string &path) {
		int wid,hig,canal;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* pixels= stbi_load(path.c_str(), &wid, &hig, &canal, 0);
		Z_CORE_ASSERT(pixels, "Failed to load image!");
		width=wid,height=hig;
		GLenum iFormat=0,dFormat=0;
		if(canal==4){
			iFormat=GL_RGBA8;
			dFormat=GL_RGBA;
		}else if(canal==3){
			iFormat=GL_RGB8;
			dFormat=GL_RGB;
		}
		internalFormat=iFormat,dataFormat=dFormat;
		Z_CORE_ASSERT(iFormat&&dFormat,"Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &ID);
		glTextureStorage2D(ID,1,iFormat,width,height);
		glTextureParameteri(ID,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTextureParameteri(ID,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTextureParameteri(ID,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTextureParameteri(ID,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTextureSubImage2D(ID,0,0,0,width,height,dFormat,GL_UNSIGNED_BYTE,pixels);
		stbi_image_free(pixels);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1,&ID);
	}

	void OpenGLTexture2D::Bind(unsigned int unit) const {
		glBindTextureUnit(unit,ID);
	}

	OpenGLTexture2D::OpenGLTexture2D(unsigned int width, unsigned int height) {
		this->width=width;
		this->height=height;
		internalFormat=GL_RGBA8;
		dataFormat=GL_RGBA;
		glCreateTextures(GL_TEXTURE_2D, 1, &ID);
		glTextureStorage2D(ID,1,GL_RGBA8,width,height);
		glTextureParameteri(ID,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTextureParameteri(ID,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTextureParameteri(ID,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTextureParameteri(ID,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

	void OpenGLTexture2D::SetData(void *data, unsigned int size) {
		unsigned int bpp=dataFormat==GL_RGBA?4:3;
		Z_CORE_ASSERT(size==width*height*bpp,"Data must be entire texture!");
		glTextureSubImage2D(ID,0,0,0,width,height,dataFormat,GL_UNSIGNED_BYTE,data);
	}

	bool OpenGLTexture2D::operator==(const Texture &other) const {
		return ID==((OpenGLTexture2D&)other).ID;
	}

}