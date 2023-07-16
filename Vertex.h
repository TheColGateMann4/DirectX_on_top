#pragma once
#include "Includes.h"
#include "Graphics.h"

namespace DynamicVertex
{
	struct RGBAColor
	{
		unsigned char r, g, b, a;
	};

	class VertexLayout
	{
	public:
		enum VertexComponent
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			RGBAColor
		};

	public:
		template<VertexComponent> struct Map;
		template<> struct Map<Position2D>
		{
			using Systype = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template<> struct Map<Position3D>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
		};
		template<> struct Map<Texture2D>
		{
			using Systype = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "TEXCOORD";
		};
		template<> struct Map<Normal>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "NORMAL";
		};
		template<> struct Map<Float3Color>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "COLOR";
		};
		template<> struct Map<Float4Color>
		{
			using Systype = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "COLOR";
		};
		template<> struct Map<RGBAColor>
		{
			using Systype = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "COLOR";
		};


	public:
		class Element
		{
		public:
			Element(VertexComponent type, size_t offset)
				: m_type(type), m_offset(offset) {}

		public:
			size_t GetOffset()	     const { return m_offset; }
			size_t GetNextOffset()   const noexcept(!IS_DEBUG) { return m_offset + m_GetTypeSize(m_type); }
			size_t GetType()	     const noexcept { return m_type; }
			size_t GetSize()	     const noexcept(!IS_DEBUG) { return m_GetTypeSize(m_type); }

			D3D11_INPUT_ELEMENT_DESC GetDirectXLayout() const noexcept(!IS_DEBUG)
			{
				switch (m_type)
				{
				case Position2D:
					return GenerateElementDesc<Position2D>(GetOffset());
				case Position3D:
					return GenerateElementDesc<Position3D>(GetOffset());
				case Texture2D:
					return GenerateElementDesc<Texture2D>(GetOffset());
				case Normal:
					return GenerateElementDesc<Normal>(GetOffset());
				case Float3Color:
					return GenerateElementDesc<Float3Color>(GetOffset());
				case Float4Color:
					return GenerateElementDesc<Float4Color>(GetOffset());
				case RGBAColor:
					return GenerateElementDesc<RGBAColor>(GetOffset());
				}
				assert("invalid element type" && false);
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
			}

		private:
			template<VertexComponent type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateElementDesc(size_t offset) noexcept(!IS_DEBUG)
			{
				return {Map<type>::semantic, 0, Map<type>::dxgiformat, 0, (UINT32)offset, D3D11_INPUT_PER_VERTEX_DATA, 0};
			}


			static constexpr size_t m_GetTypeSize(VertexComponent type) noexcept(!IS_DEBUG)
			{
				switch (type)
				{
				case Position2D:
					return sizeof(Map<Position2D>::Systype);
				case Position3D:
					return sizeof(Map<Position3D>::Systype);
				case Texture2D:
					return sizeof(Map<Texture2D>::Systype);
				case Normal:
					return sizeof(Map<Normal>::Systype);
				case Float3Color:
					return sizeof(Map<Float3Color>::Systype);
				case Float4Color:
					return sizeof(Map<Float4Color>::Systype);
				case RGBAColor:
					return sizeof(Map<RGBAColor>::Systype);
				}
				assert("invalid element type" && false);
				return false;
			}

		private:
			VertexComponent m_type;
			size_t m_offset;
		};

	public:
		template<VertexComponent type>
		const Element& Resolve() const noexcept(!IS_DEBUG)
		{
			for (auto& element : m_elements)
				if (element.GetType() == type)
					return element;
			assert("Couldn't resolve element type" && false);
			return m_elements.front();
		}

		const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
		{
			return m_elements.at(i);
		}

		VertexLayout& Append(VertexComponent type) noexcept(!IS_DEBUG)
		{
			m_elements.emplace_back(type, GetByteSize());
			return *this;
		}

		size_t GetByteSize() const noexcept(!IS_DEBUG)
		{
			return m_elements.empty() ? 0 : m_elements.back().GetNextOffset();
		}

		size_t GetElementCount() const noexcept
		{
			return m_elements.size();
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> GetDirectXLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> result;
			result.reserve(GetElementCount());

			for (auto& element : m_elements)
				result.push_back(element.GetDirectXLayout());

			return result;
		}

	private:
		std::vector<Element> m_elements;
	};

	class Vertex
	{
		friend class VertexBuffer;

	protected:
		Vertex(char* pData, const VertexLayout& layout)
			: m_pData(pData), m_layout(layout)
		{
			assert(m_pData != nullptr);
		}

	public:
		template<VertexLayout::VertexComponent type>
		auto& Attr() noexcept(!IS_DEBUG)
		{
			auto pAttrib = m_pData + m_layout.Resolve<type>().GetOffset();
			return *reinterpret_cast<class VertexLayout::Map<type>::Systype*>(pAttrib);
		}

		template<VertexLayout::VertexComponent DestLayoutType, class SrcType>
		void SetAttr(char* pAttr, SrcType&& val) noexcept(!IS_DEBUG)
		{
			using DestType = typename VertexLayout::Map<DestLayoutType>::Systype;

			if constexpr (std::is_assignable_v<DestType, SrcType>)
			{
				*reinterpret_cast<DestType*>(pAttr) = val;
			}
			else
			{
				assert("type attribute didn't match" && false);
			}
		}

		template<class T>
		void SetAttrByIndex(size_t i, T&& val) noexcept(!IS_DEBUG)
		{
			const auto& element = m_layout.ResolveByIndex(i);
			auto pAttrib = m_pData + element.GetOffset();

			switch (element.GetType())
			{
			case VertexLayout::Position2D:
			{
				SetAttr<VertexLayout::Position2D>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::Position3D:
			{
				SetAttr<VertexLayout::Position3D>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::Texture2D:
			{
				SetAttr<VertexLayout::Texture2D>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::Normal:
			{
				SetAttr<VertexLayout::Normal>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::Float3Color:
			{
				SetAttr<VertexLayout::Float3Color>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::Float4Color:
			{
				SetAttr<VertexLayout::Float4Color>(pAttrib, std::forward<T>(val));
				break;
			}
			case VertexLayout::RGBAColor:
			{
				SetAttr<VertexLayout::RGBAColor>(pAttrib, std::forward<T>(val));
				break;
			}
			default:
			{
				assert("Unknown element type" && false);
			}
			}
		}
	private:
		template<class First, class ...Rest>
		void SetAttrByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG)
		{
			SetAttrByIndex(i, std::forward<First>(first));
			SetAttrByIndex(i + 1, std::forward<Rest>(rest)...);
		}

	private:
		char* m_pData = nullptr;
		const VertexLayout& m_layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex(const Vertex& vertex) noexcept(!IS_DEBUG)
			:m_vertex(vertex) {}

	public:
		template<VertexLayout::VertexComponent type>
		const auto& Attr() const noexcept(!IS_DEBUG)
		{
			return const_cast<Vertex&>(m_vertex).Attr<type>();
		}

	private:
		Vertex m_vertex;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
			: m_layout(std::move(layout)) {}

	public:
		const char* GetData() const noexcept(!IS_DEBUG)
		{
			return m_buffer.data();
		}

		const VertexLayout& GetLayout() const noexcept
		{
			return m_layout;
		}

		//size in vertices
		size_t GetSize() const noexcept(!IS_DEBUG)
		{
			return m_buffer.size() / m_layout.GetByteSize();
		}

		//size in bytes
		size_t GetBytesSize() const noexcept(!IS_DEBUG)
		{
			return m_buffer.size();
		}

		template<class ...Params>
		void Emplace_Back(Params&&... params) noexcept(!IS_DEBUG)
		{
			assert(sizeof...(params) == m_layout.GetElementCount() && "Number of parameters doesn't match the specified structure");
			m_buffer.resize(m_buffer.size() + m_layout.GetByteSize());
			this->Back().SetAttrByIndex(0, std::forward<Params>(params)...);
		}

		//Vertex stuff
		Vertex Back() noexcept(!IS_DEBUG)
		{
			assert(m_buffer.size() != 0);
			return Vertex{
				m_buffer.data() + m_buffer.size() - m_layout.GetByteSize(),
				m_layout
			};
		}

		Vertex Front() noexcept(!IS_DEBUG)
		{
			assert(m_buffer.size() != 0);
			return Vertex{
				m_buffer.data(),
				m_layout
			};
		}

		Vertex operator[](size_t i) noexcept(!IS_DEBUG)
		{
			assert(i < m_buffer.size());
			return Vertex{ m_buffer.data() + (i * m_layout.GetByteSize()) , m_layout };
		}

		//Const Vertex Stuff
		ConstVertex Back() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Back();
		}

		ConstVertex Front() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}

		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}

	private:
		std::vector<char> m_buffer;
		VertexLayout m_layout;
	};
}