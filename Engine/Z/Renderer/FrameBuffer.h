//
// Created by 32725 on 2023/3/23.
//

#ifndef ENGINE_TUTORIAL_FRAMEBUFFER_H
#define ENGINE_TUTORIAL_FRAMEBUFFER_H

#include <vector>
#include "Z/Core/Core.h"

namespace Z {

	enum class FrameBufferTextureFormat {
		None = 0,
		R32I,
		R32F,
		RGBA8,
		RGBA32F,
		DEPTH24STENCIL8,
		DEPTH=DEPTH24STENCIL8};

	struct FrameBufferFormatSpec{
		FrameBufferFormatSpec()=default;
		FrameBufferFormatSpec(FrameBufferTextureFormat format): format(format){}

		FrameBufferTextureFormat format=FrameBufferTextureFormat::None;
	};

	struct FrameBufferAttachmentsSpec{
		FrameBufferAttachmentsSpec()=default;
		FrameBufferAttachmentsSpec(std::initializer_list<FrameBufferFormatSpec> _formats): formats(_formats){}

		std::vector<FrameBufferFormatSpec> formats;
	};

	struct FrameBufferSpecification {
		unsigned int width, height;
		FrameBufferAttachmentsSpec attachments;
		unsigned int samples = 1;
		bool swapChainTarget = false;
	};

	class FrameBuffer {
	public:
		static Ref<FrameBuffer> Create(const FrameBufferSpecification &specification);
		inline virtual void Bind() const=0;
		inline virtual void UnBind() const=0;
		virtual void Resize(unsigned int width, unsigned int height) = 0;
		inline virtual unsigned int GetAttachmentID(unsigned int num) const = 0;
		virtual void ClearAttachment(unsigned int num, int value) = 0;
		virtual int GetPixel(unsigned int x, unsigned int y,unsigned int id=1)=0;
		virtual const FrameBufferSpecification &GetSpecification() const = 0;
		virtual FrameBufferSpecification &GetSpecification() = 0;

		virtual ~FrameBuffer() = default;
	};

}


#endif //ENGINE_TUTORIAL_FRAMEBUFFER_H
