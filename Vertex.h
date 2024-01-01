#pragma once
#include "Includes.h"
#include "Graphics.h"
#include <assimp/scene.h>

#define FOR_ALL_VERTEX_ELEMENTS(additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Position2D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Position3D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Tangent, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Bitangent, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Texture2D, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Normal, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Float3Color, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::Float4Color, additionalStatement) \
	STATEMENT(DynamicVertex::VertexLayout::VertexComponent::RGBAColor, additionalStatement)

#define MAKE_DATA_FOR_VERTEX_COMPONENT(componentFunction) \
	static Systype GetData(const aiMesh& mesh, const size_t i, float vertexScale)	\
	{	\
		return *reinterpret_cast<Systype*>(&mesh.componentFunction[i]);	\
	}

#define MAKE_DATA_FOR_VERTEX_COMPONENT_POSITION3D(componentFunction) \
	static Systype GetData(const aiMesh& mesh, const size_t i, float vertexScale)	\
	{	\
		return Systype(mesh.componentFunction[i].x * vertexScale, mesh.componentFunction[i].y * vertexScale, mesh.componentFunction[i].z * vertexScale);	\
	}

namespace DynamicVertex
{
	struct RGBAColor
	{
		unsigned char r, g, b, a;
	};

	class VertexLayout
	{
		friend class VertexBuffer;
	public:
		enum VertexComponent
		{
			Position2D,
			Position3D,
			Tangent,
			Bitangent,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			RGBAColor,
			VerticesNum
		};

	public:
		template<VertexComponent> struct Map
		{
			using Systype = void;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "INVALID";
			static constexpr const char* code = "00";
			static constexpr size_t size = 0;
		};

		template<> struct Map<Position2D>
		{
			using Systype = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "POSITION";
			static constexpr const char* code = "P2";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mVertices)
		};
		template<> struct Map<Position3D>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "POSITION";
			static constexpr const char* code = "P3";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT_POSITION3D(mVertices)
		};
		template<> struct Map<Tangent>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "TANGENT";
			static constexpr const char* code = "T";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mTangents)
		};
		template<> struct Map<Bitangent>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "BITANGENT";
			static constexpr const char* code = "BT";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mBitangents)
		};
		template<> struct Map<Texture2D>
		{
			using Systype = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "TEXCOORD";
			static constexpr const char* code = "T2";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mTextureCoords[0])
		};
		template<> struct Map<Normal>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "NORMAL";
			static constexpr const char* code = "N";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mNormals)
		};
		template<> struct Map<Float3Color>
		{
			using Systype = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "COLOR";
			static constexpr const char* code = "C3";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mColors[0])
		};
		template<> struct Map<Float4Color>
		{
			using Systype = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "COLOR";
			static constexpr const char* code = "C4";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mColors[0])
		};
		template<> struct Map<RGBAColor>
		{
			using Systype = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "COLOR";
			static constexpr const char* code = "RGBA";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mColors[0])
		};
		template<> struct Map<VerticesNum>
		{
			using Systype = size_t;
			static constexpr DXGI_FORMAT dxgiformat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "INVALID";
			static constexpr const char* code = "00";
			static constexpr size_t size = sizeof(Systype);

			MAKE_DATA_FOR_VERTEX_COMPONENT(mFaces)
		};

	public:
		enum ComponentGrabberType
		{
			GetSystype,
			GetDXGIFormat,
			GetSemantic,
			GetCode,
			GetSize
		};

		template<ComponentGrabberType type> struct GetVertexComponentVarible;
		#define STATEMENT(vertexComponent, vertexComponentMember)\
			case vertexComponent:	\
			{	\
				return Map<vertexComponent>::vertexComponentMember;	\
			}	\

		#define MAKE_COMPONENT_GRABBER_FOR_TYPE(enumGrabberType, grabberReturnType, vertexComponentMember)\
		template<> struct GetVertexComponentVarible<enumGrabberType>	\
		{	\
			static constexpr grabberReturnType Get(VertexComponent type)	\
			{	\
				switch (type)	\
				{	\
					FOR_ALL_VERTEX_ELEMENTS(vertexComponentMember)	\
				}	\
				return {};	\
			}	\
		};

		MAKE_COMPONENT_GRABBER_FOR_TYPE(GetDXGIFormat, DXGI_FORMAT, dxgiformat)
		MAKE_COMPONENT_GRABBER_FOR_TYPE(GetSemantic, const char*, semantic)
		MAKE_COMPONENT_GRABBER_FOR_TYPE(GetCode, const char*, semantic)
		MAKE_COMPONENT_GRABBER_FOR_TYPE(GetSize, size_t, size)

		#undef MAKE_COMPONENT_GRABBER_FOR_TYPE
		#undef STATEMENT

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
				#define STATEMENT(vertexComponent, additionalComponent) \
					case vertexComponent: \
					{ \
						return GenerateElementDesc<vertexComponent>(GetOffset()); \
					}

				switch (m_type)
				{
					FOR_ALL_VERTEX_ELEMENTS(void);
					#undef STATEMENT
				}

				assert("invalid element type" && false);
				return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
			}

			const char* GetCode() const noexcept
			{
				return GetVertexComponentVarible<ComponentGrabberType::GetCode>::Get(m_type);
			}

		private:
			template<VertexComponent type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateElementDesc(size_t offset) noexcept
			{
				return {Map<type>::semantic, 0, Map<type>::dxgiformat, 0, (UINT32)offset, D3D11_INPUT_PER_VERTEX_DATA, 0};
			}


			static constexpr size_t m_GetTypeSize(VertexComponent type) noexcept(!IS_DEBUG)
			{
				return GetVertexComponentVarible<ComponentGrabberType::GetSize>::Get(type);
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

		const Element& GetByIndex(size_t i) const noexcept(!IS_DEBUG)
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

		size_t GetStructureSize() const noexcept(!IS_DEBUG)
		{
			return GetByteSize() / GetElementCount();
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

		std::string GetUID() const
		{
			std::string result = {};

			for (auto& element : m_elements)
				result += element.GetCode();

			return result;
		}

	private:
		std::vector<Element> m_elements;
	};

	class VertexBuffer;

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
		auto& Attribute() noexcept(!IS_DEBUG)
		{
			auto pAttrib = m_pData + m_layout.Resolve<type>().GetOffset();
			return *reinterpret_cast<class VertexLayout::Map<type>::Systype*>(pAttrib);
		}

		template<VertexLayout::VertexComponent DestLayoutType, class SrcType>
		void SetAttribute(char* pAttr, SrcType&& val) noexcept(!IS_DEBUG)
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
		void SetAttributeByIndex(size_t i, T&& val) noexcept(!IS_DEBUG)
		{
			const auto& element = m_layout.GetByIndex(i);
			auto pAttrib = m_pData + element.GetOffset();


#define STATEMENT(vertexComponent, additionalComponent)\
				case vertexComponent:	\
				{	\
					SetAttribute<vertexComponent>(pAttrib, std::forward<T>(val));	\
					break;	\
				}

			switch (element.GetType())
			{
				FOR_ALL_VERTEX_ELEMENTS(void);

				default:
				{
					assert("Unknown element type" && false);
				}
#undef STATEMENT
			}
		}
	private:
		template<class First, class ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG)
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
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
			return const_cast<Vertex&>(m_vertex).Attribute<type>();
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
		const char* GetConstData() const noexcept(!IS_DEBUG)
		{
			return m_buffer.data();
		}
		char* GetData() noexcept(!IS_DEBUG)
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

		// might be unsafe
		void Emplace_Back_Empty(size_t numberOfVertices) noexcept(!IS_DEBUG)
		{
			const size_t layoutSize = m_layout.GetByteSize();
			const size_t addedSize = layoutSize * numberOfVertices;

			m_buffer.resize(m_buffer.size() + addedSize);
			memset(static_cast<void*>((m_buffer.data() + m_buffer.size()) - addedSize), '\0', addedSize);
		}

		template<class ...Params>
		void Emplace_Back(Params&&... params) noexcept(!IS_DEBUG)
		{
			assert(sizeof...(params) == m_layout.GetElementCount() && "Number of parameters doesn't match the specified structure");
			m_buffer.resize(m_buffer.size() + m_layout.GetByteSize());
			this->Back().SetAttributeByIndex(0, std::forward<Params>(params)...);
		}

		void MakeFromMesh(const aiMesh& mesh)
		{
			size_t numberOfVertices = mesh.mNumVertices;

			this->Emplace_Back_Empty(numberOfVertices);

			for (size_t i = 0; i < numberOfVertices; i++)
			{
				size_t currentElementIndex = 0;
				for (const auto& element : m_layout.m_elements)
				{
					#define STATEMENT(vertexComponent, additionalComponent) \
					case vertexComponent:	\
					{	\
						(*this)[i].SetAttribute<vertexComponent>(	\
							this->GetData() + this->GetLayout().GetByteSize() * i + this->GetLayout().GetByIndex(currentElementIndex).GetOffset(),	\
							::DynamicVertex::VertexLayout::Map<vertexComponent>::GetData(mesh, i, vertexScale)	\
						);	\
						break;	\
					}

					switch (element.GetType())
					{
					
						FOR_ALL_VERTEX_ELEMENTS(void);
						#undef STATEMENT
					}
					currentElementIndex++;
				}
			}
		}

		void SetScale(float scale)
		{
			vertexScale = scale;
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
			assert(m_buffer.size() / m_layout.GetByteSize() != 0);
			return Vertex{
				m_buffer.data(),
				m_layout
			};
		}

		Vertex operator[](size_t i) noexcept(!IS_DEBUG)
		{
			assert(i < m_buffer.size() / m_layout.GetByteSize());
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
		float vertexScale = 1.0f;
	};
}

#undef FOR_ALL_VERTEX_ELEMENTS;
#undef MAKE_DATA_FOR_VERTEX_COMPONENT;
#undef MAKE_DATA_FOR_VERTEX_COMPONENT_POSITION3D;