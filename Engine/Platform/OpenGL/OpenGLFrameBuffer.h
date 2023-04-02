//
// Created by 32725 on 2023/3/23.
//

#ifndef ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H
#define ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H

#include "Z/Renderer/FrameBuffer.h"
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "Z/Core/Log.h"

namespace Z {


	class OpenGLFrameBuffer : public FrameBuffer {
		unsigned int ID;
		unsigned int DepthID;
		std::vector<unsigned int> Attachments;
		std::vector<FrameBufferFormatSpec> AttachmentsSpecs;
		FrameBufferFormatSpec depthSpec;
		FrameBufferSpecification specification;

		void Generate();

	public:
		OpenGLFrameBuffer(const FrameBufferSpecification &specification);

		virtual ~OpenGLFrameBuffer() override {
			glDeleteFramebuffers(1, &ID);
			glDeleteTextures(Attachments.size(), Attachments.data());
			glDeleteTextures(1, &DepthID);
		}

		inline virtual void Bind() const override {
			glBindFramebuffer(GL_FRAMEBUFFER, ID);
			glViewport(0, 0, specification.width, specification.height);
		}

		inline virtual void UnBind() const override { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		virtual void Resize(unsigned int width, unsigned int height) override;

		inline virtual unsigned int GetAttachmentID(unsigned int num) const override {
			Z_CORE_ASSERT(num < Attachments.size(), "Out of range")
			return Attachments[num];
		}

		inline virtual const FrameBufferSpecification &GetSpecification() const override { return specification; }

		inline virtual FrameBufferSpecification &GetSpecification() override { return specification; }

		virtual int GetPixel(unsigned int x, unsigned int y, unsigned int id) override;
		virtual void ClearAttachment(unsigned int num, int value) override;

	};

}


#endif //ENGINE_TUTORIAL_OPENGLFRAMEBUFFER_H
