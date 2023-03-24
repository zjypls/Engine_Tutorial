//
// Created by 32725 on 2023/3/23.
//

#ifndef ENGINE_TUTORIAL_FRAMEBUFFER_H
#define ENGINE_TUTORIAL_FRAMEBUFFER_H
#include "Z/Core/Core.h"

namespace Z {
	struct FrameBufferSpecification {
		unsigned int width, height;
		unsigned int samples = 1;
		bool swapChainTarget = false;
	};

	class FrameBuffer {
	public:
		static Ref<FrameBuffer> Create(const FrameBufferSpecification &specification);
		inline virtual void Bind() const=0;
		inline virtual void UnBind() const=0;
		virtual void Resize(unsigned int width, unsigned int height) = 0;
		inline virtual unsigned int GetColorID() const = 0;
		inline virtual unsigned int GetDepthID() const = 0;
		virtual const FrameBufferSpecification &GetSpecification() const = 0;
		virtual FrameBufferSpecification &GetSpecification() = 0;

		virtual ~FrameBuffer() = default;
	};

}


#endif //ENGINE_TUTORIAL_FRAMEBUFFER_H
