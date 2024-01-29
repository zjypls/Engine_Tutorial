//
// Created by 32725 on 2023/3/23.
//

#include "Platform/OpenGL/OpenGLFrameBuffer.h"

constexpr unsigned int MAX_FRAMEBUFFER_SIZE = 8192;
namespace Z {


	namespace Tools {
		static GLenum GetTarget(bool samples) {
			return samples ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static bool IsDepthAttachment(FrameBufferTextureFormat format) {
			switch (format) {
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
					return true;
				default:
					return false;
			}
		}


		static void BindTexture(bool multiSample, unsigned int id) {
			glBindTexture(GetTarget(multiSample), id);
		}

		static void
		AttachColor(unsigned int id, GLenum internalFormat, GLenum format, int samples, int wid, int hig, int num) {
			const bool multiSample = samples > 1;
			BindTexture(multiSample, id);
			if (multiSample) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, wid, hig, GL_FALSE);
			} else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, wid, hig, 0, format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + num, GetTarget(multiSample), id, 0);
		}

		static void AttachDepth(unsigned int id, GLenum format, GLenum attachType, int samples, int wid, int hig) {
			const bool multiSample = samples > 1;
			BindTexture(multiSample, id);
			if (multiSample) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, wid, hig, GL_FALSE);
			} else {
				glTexStorage2D(GL_TEXTURE_2D, 1, format, wid, hig);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GetTarget(multiSample), id, 0);
		}

		static void CreateTextures(bool multiSampler, GLuint *data, unsigned int size) {
			glCreateTextures(GetTarget(multiSampler), size, data);
		}


	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification &specification) : specification(specification) {

		for (auto &spec: specification.attachments.formats) {
			if (!Tools::IsDepthAttachment(spec.format))
				AttachmentsSpecs.emplace_back(spec);
			else
				depthSpec = spec;
		}
		Attachments.resize(AttachmentsSpecs.size());
		Generate();
	}

	void OpenGLFrameBuffer::Resize(unsigned int width, unsigned int height) {
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE) {
			Z_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		specification.width = width;
		specification.height = height;

		glDeleteTextures(Attachments.size(), Attachments.data());
		glDeleteTextures(1, &DepthID);
		Attachments.clear();
		Attachments.resize(AttachmentsSpecs.size());
		DepthID = 0;
		glDeleteFramebuffers(1, &ID);
		Generate();
	}

	void OpenGLFrameBuffer::Generate() {
		glCreateFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		const bool multiSample = specification.samples > 1;
		Tools::CreateTextures(multiSample, Attachments.data(), Attachments.size());

		for (int index = 0; index < Attachments.size(); ++index) {
			switch (AttachmentsSpecs[index].format) {
				case FrameBufferTextureFormat::R32I:
					Tools::AttachColor(Attachments[index], GL_R32I, GL_RED_INTEGER, specification.samples,
					                   specification.width, specification.height, index);
					break;
				case FrameBufferTextureFormat::R32F:
					Tools::AttachColor(Attachments[index], GL_R32F, GL_RED, specification.samples, specification.width,
					                   specification.height, index);
					break;
				case FrameBufferTextureFormat::RGBA8:
					Tools::AttachColor(Attachments[index], GL_RGBA, GL_RGBA, specification.samples, specification.width,
					                   specification.height, index);
					break;
				case FrameBufferTextureFormat::RGBA32F:
					Tools::AttachColor(Attachments[index], GL_RGBA32F, GL_RGBA, specification.samples,
					                   specification.width, specification.height, index);
					break;
				default: Z_CORE_ASSERT(false, "Unknown format");
			}
		}
		if (depthSpec.format != FrameBufferTextureFormat::None) {
			Tools::CreateTextures(multiSample, &DepthID, 1);
			switch (depthSpec.format) {
				case FrameBufferTextureFormat::DEPTH24STENCIL8:
					Tools::AttachDepth(DepthID, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, specification.samples,
					                   specification.width, specification.height);
					break;
				default: Z_CORE_ASSERT(false, "Unknown format");
			}
		}

		Z_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
		              "Framebuffer is incomplete!");
		if (Attachments.empty())
			glDrawBuffer(GL_NONE);
		else {
			std::vector<GLenum> buffers(Attachments.size());
			for (int i = 0; i < buffers.size(); ++i) {
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(buffers.size(), buffers.data());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGLFrameBuffer::GetPixel(unsigned int x, unsigned int y, unsigned int id) {
		Z_CORE_ASSERT(id < Attachments.size(), "Invalid id");
		int color = -1;
		glReadBuffer(GL_COLOR_ATTACHMENT0 + id);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &color);
		return color;
	}

	void OpenGLFrameBuffer::ClearAttachment(unsigned int num, int value) {
		Z_CORE_ASSERT(num < Attachments.size(), "Invalid id");
		glClearTexImage(Attachments[num], 0, GL_RED_INTEGER, GL_INT, &value);
	}

}