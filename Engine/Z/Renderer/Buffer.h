//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_BUFFER_H
#define ENGINE_TUTORIAL_BUFFER_H
#include <string>
#include <cstdint>
#include <vector>
#include "Z/Core/Core.h"
#include "Z/Core/Log.h"

namespace Z {
	enum class ShaderDataType {
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	uint32_t GetShaderDataTypeSize(ShaderDataType type);


	struct BufferElement{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string &name, bool normalized = false)
				: Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {
		}
		inline uint32_t GetOffset() const { return Offset; }

		uint32_t GetComponentCount() const ;
	};

	class Z_API BufferLayout {
		std::vector<BufferElement> Elements;
		uint32_t Stride = 0;
		void CalculateOffsetsAndStride();
		public:
		BufferLayout() = default;
		uint32_t GetElementCount() const { return Elements.size(); }
		uint32_t GetStride() const { return Stride; }

		BufferLayout(const std::initializer_list<BufferElement> &elements)
				: Elements(elements) {
			CalculateOffsetsAndStride();
		}

		inline const std::vector<BufferElement> &GetElements() const { return Elements; }
		auto begin() { return Elements.begin(); }
		auto end() { return Elements.end(); }
		auto begin() const { return Elements.begin(); }
		auto end() const { return Elements.end(); }
	};

	class Z_API VertexBuffer {

	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;
		virtual void SetLayout(const BufferLayout &layout) = 0;
		virtual void SetData(const void *vertices, uint32_t size) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static Ref<VertexBuffer> Create(float *vertices, uint32_t size);
		static Ref<VertexBuffer> Create(uint32_t size);

	};

	class Z_API IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);
	};
}


#endif //ENGINE_TUTORIAL_BUFFER_H
