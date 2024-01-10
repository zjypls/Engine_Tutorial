//
// Created by 32725 on 2023/3/18.
//
#define STB_IMAGE_IMPLEMENTATION
#include "Include/stb/stb_image.h"
#include "Include/glad/include/glad/glad.h"

#include "Z/Core/Log.h"
#include "Platform/OpenGL/OpenGLTextures.h"

namespace Z {
	OpenGLTexture2D::OpenGLTexture2D(const std::string &path) {
		int wid,hig,canal;
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



	void OpenGLSkyBox::SetData(void *data, unsigned int size) {

	}

	OpenGLSkyBox::OpenGLSkyBox(const std::vector<std::string> &paths) {
		Z_CORE_ASSERT(paths.size()==6,"Cube map textures sources size should be 6!!");
		glCreateTextures(GL_TEXTURE_CUBE_MAP,1,&ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP,ID);
		int width=0,height=0,comp;
		stbi_set_flip_vertically_on_load(0);
		for(int i=0;i<6;++i){
			stbi_uc *pixels= stbi_load(paths[i].c_str(),&width,&height,&comp,0);
			Z_CORE_ASSERT(pixels!= nullptr,paths[i]);
			glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels
			);
			stbi_image_free(pixels);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		this->width=width;
		this->height=height;
		stbi_set_flip_vertically_on_load(1);

	}

	OpenGLSkyBox::OpenGLSkyBox(unsigned int width, unsigned int height): width(width), height(height) {
		glCreateTextures(GL_TEXTURE_CUBE_MAP,1,&ID);
		for(int i=0;i<6;++i){
			glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB,this->width,this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void OpenGLTexture3D::SetData(void *data, unsigned int size) {

	}

	OpenGLTexture3D::OpenGLTexture3D(int width, int height, int depth)
	:width(width),height(height),depth(depth){
	}
}