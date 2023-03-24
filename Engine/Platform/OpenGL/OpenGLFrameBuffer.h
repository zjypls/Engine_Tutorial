//
// Created by 32725 on 2023/3/23.
//

#ifndef ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H
#define ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H
#include "Z/Renderer/FrameBuffer.h"
#include "glad/glad.h"
#include "Z/Core/Log.h"
namespace Z {
	class OpenGLFrameBuffer: public FrameBuffer {
		unsigned int ID;
		unsigned int colorID, depthID;
		FrameBufferSpecification specification;
		void Generate();
		public:
			OpenGLFrameBuffer(const FrameBufferSpecification &specification):specification(specification){Generate();}
			virtual ~OpenGLFrameBuffer() override{glDeleteFramebuffers(1, &ID);glDeleteTextures(1, &colorID);glDeleteTextures(1, &depthID);}
			inline virtual void Bind() const override{glBindFramebuffer(GL_FRAMEBUFFER, ID);glViewport(0, 0, specification.width, specification.height);}
			inline virtual void UnBind() const override{glBindFramebuffer(GL_FRAMEBUFFER, 0);}
			virtual void Resize(unsigned int width, unsigned int height) override;
			inline virtual unsigned int GetColorID() const override{ return colorID;}
			inline virtual unsigned int GetDepthID() const override{ return depthID;}
			inline virtual const FrameBufferSpecification &GetSpecification() const override{ return specification;}
			inline virtual FrameBufferSpecification &GetSpecification() override{ return specification;}

	};

}


#endif //ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H
