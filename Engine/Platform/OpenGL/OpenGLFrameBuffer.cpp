//
// Created by 32725 on 2023/3/23.
//

#include "OpenGLFrameBuffer.h"
constexpr unsigned int MAX_FRAMEBUFFER_SIZE = 8192;
namespace Z {
	void OpenGLFrameBuffer::Resize(unsigned int width, unsigned int height) {
		if(width==0||height==0||width>MAX_FRAMEBUFFER_SIZE||height>MAX_FRAMEBUFFER_SIZE){
			Z_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		specification.width = width;
		specification.height = height;

		glDeleteTextures(1, &colorID);
		glDeleteTextures(1, &depthID);
		glDeleteFramebuffers(1, &ID);
		Generate();
	}

	void OpenGLFrameBuffer::Generate() {
		glCreateFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
		glCreateTextures(GL_TEXTURE_2D, 1, &colorID);
		glBindTexture(GL_TEXTURE_2D, colorID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification.width, specification.height, 0, GL_RGBA,
		             GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorID, 0);
		glCreateTextures(GL_TEXTURE_2D, 1, &depthID);
		glBindTexture(GL_TEXTURE_2D, depthID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification.width, specification.height, 0,
		             GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);
		Z_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
		              "Framebuffer is incomplete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}