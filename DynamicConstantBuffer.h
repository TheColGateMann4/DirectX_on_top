#pragma once
#include <vector>
#include <DirectXMath.h>
#include <windows.h>
#include <string>
#include <memory>
#include <iostream>



#define FOR_ALL_TYPES STATEMENT(Float) \
	STATEMENT(Float2) \
	STATEMENT(Float3) \
	STATEMENT(Float4) \
	STATEMENT(Matrix) \
	STATEMENT(Bool)


namespace DynamicConstantBuffer
{

	enum DataType
	{
		Empty,

#define STATEMENT(enum_element) enum_element,
		FOR_ALL_TYPES
#undef STATEMENT

		Struct,
		Array
	};



	template<DataType type>
	struct DataTypeMap // Empty, Array and Struct will be invalid
	{
		static constexpr bool valid = false;
	};
	template<> struct DataTypeMap<DataType::Float>
	{
		using type = float;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "F1";
	};
	template<> struct DataTypeMap<DataType::Float2>
	{
		using type = DirectX::XMFLOAT2;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "F2";
	};
	template<> struct DataTypeMap<DataType::Float3>
	{
		using type = DirectX::XMFLOAT3;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "F3";
	};
	template<> struct DataTypeMap<DataType::Float4>
	{
		using type = DirectX::XMFLOAT4;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "F4";
	};
	template<> struct DataTypeMap<DataType::Matrix>
	{
		using type = DirectX::XMMATRIX;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "MA";
	};
	template<> struct DataTypeMap<DataType::Bool>
	{
		using type = bool;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = 4;
		static constexpr const char* code = "BO";
	};


	
#define STATEMENT(enum_element) static_assert(DataTypeMap<enum_element>::valid, "missing definition for " #enum_element);
	FOR_ALL_TYPES
#undef STATEMENT



	class LayoutElement;

	namespace AdditionalElements
	{
		struct AdditionalDataBase
		{

		};
		struct ArrayData : public AdditionalDataBase
		{
			DataType type = DataType::Empty;
			size_t size;
		};
		struct StructData : public AdditionalDataBase
		{
			::std::vector<::std::pair<::std::string, std::shared_ptr<LayoutElement>>> data = {};
		};
	}


	class LayoutElement
	{
		friend class BufferLayout;
	public:
		LayoutElement(DataType elementType);

		LayoutElement();

		~LayoutElement();

	public:
		template<DataType type>
		void Add(const char* elementName) // for DataType::Struct
		{
			assert(this->m_type == Struct, "Cannot add places to not Struct element, element was type " + this->m_type);

			auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*m_additionalData);

			additionalData.data.push_back({ elementName, std::make_shared<LayoutElement>(type)});
		}

		template<DataType type>
		void Add(size_t additionaLenght = 1) // for DataType::Array; additionaLenght = number of added space
		{
			assert(this->m_type == Array, "Cannot add space to not Array element, element was type " + this->m_type);

			auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData&>(*m_additionalData);

			if (additionalData.type != DynamicConstantBuffer::DataType::Empty) // we can do it this was since user won't be using array of empty elements
				assert(additionalData.type == type, "Assigning element to array with different type");

			additionalData.type = type;
			additionalData.size++;
		}

	public:
		const LayoutElement* operator[] (const char* elementName) noexcept;

	public:
		DataType GetType() const { return m_type; };

	private:
		DataType m_type = DataType::Empty;
		::std::unique_ptr<AdditionalElements::AdditionalDataBase> m_additionalData = {};
	};


	class BufferLayout
	{
	public:
		BufferLayout(const char* bufferIdentificator);

		BufferLayout();

		~BufferLayout();

	public:
		template<DataType type>
		void Add(const char* elementName)
		{
			m_elements.push_back({ elementName, std::make_shared<LayoutElement>(type) });
		}

	public:
		bool isExisting(const char* elementName);

		size_t GetOffsetOfElement(const char* elementName) const;

		const BufferLayout& GetFinished(size_t* newSize = nullptr, std::vector<size_t>* addedSpacesOffsets = nullptr);

		static size_t GetLayoutSize(const BufferLayout& layout);

	public:
		const auto& GetVector() const
		{
			return m_elements;
		};

		std::string GetIdentificator() const;

		void MakeBufferFromIdentificator(const char* identificator, bool calledByFunction = false, DynamicConstantBuffer::AdditionalElements::StructData* vectorData = nullptr);

	public:
		BufferLayout operator= (const char* identificator) noexcept;
		BufferLayout operator= (const BufferLayout& layout) noexcept;

		LayoutElement* operator[] (const char* elementName) noexcept;
		const LayoutElement* operator[] (const char* elementName) const noexcept;

	public:
		static size_t GetElementSize(DataType type);
		static const char* GetElementCode(DataType type);

	private:
		::std::vector<::std::pair<::std::string, std::shared_ptr<LayoutElement>>> m_elements;
	};




	class BufferData
	{
	public:
		BufferData(const BufferLayout& layout);

		BufferData(const BufferData& bufferToAssign);

		BufferData();

		~BufferData();

	public:
		template<class T, std::enable_if_t<!std::is_pointer_v<T>, int> = 0>
		BufferData& operator+=(const T& newBufferData)
		{
 			size_t tsize = sizeof(T);
 			assert(latestUseOfAdditionOperator + tsize <= m_size && "attempted to write too much bytes to DCB");
			
			void* dest = static_cast<void*>(static_cast<char*>(m_pDataBuffer) + latestUseOfAdditionOperator);
			const void* src = static_cast<const void*>(&newBufferData);

			memcpy_s(dest, m_size, src, tsize);

			latestUseOfAdditionOperator += tsize;

			return *this;
		}

		template<class T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
		BufferData& operator+=(const T newBufferData)
		{
			*this += *newBufferData;
		}

		BufferData& operator=(const BufferData& bufferData)
		{
			m_layout = bufferData.m_layout;
			m_size = bufferData.m_size;

			if(m_pDataBuffer != nullptr)
				delete[] m_pDataBuffer;

			m_pDataBuffer = new char[m_size];

			memcpy_s(m_pDataBuffer, m_size, bufferData.m_pDataBuffer, bufferData.m_size);

			return *this;
		}

	public:
		bool ElementExists(const char* elementName)
		{
			return m_layout.isExisting(elementName);
		}

		template<DataType type>
		DynamicConstantBuffer::DataTypeMap<type>::type* GetElementPointerValue(const char* elementName) const
		{
			return static_cast<DynamicConstantBuffer::DataTypeMap<type>::type*>(static_cast<void*>(static_cast<char*>(m_pDataBuffer) + m_layout.GetOffsetOfElement(elementName)));
		}

		template<DataType type>
		DynamicConstantBuffer::DataTypeMap<type>::type* GetElementPointerValue(const char* elementName)
		{
			return static_cast<DynamicConstantBuffer::DataTypeMap<type>::type*>(static_cast<void*>(static_cast<char*>(m_pDataBuffer) + m_layout.GetOffsetOfElement(elementName)));
		}

	public:
		void MakeFinished();

	public:
		BufferLayout& GetMutableLayout();
		const BufferLayout& GetConstLayout() const;

	public:
		template<DataType type>
		void AddLayoutElement(const char* elementName)
		{
			m_layout.Add<type>(elementName);
			m_size += DynamicConstantBuffer::DataTypeMap<type>::hlslsize;

			m_pDataBuffer = realloc(m_pDataBuffer, m_size);

			assert(m_pDataBuffer != nullptr);
		}

	public:
		void Update(const BufferLayout layout);

#ifdef _DEBUG
		void DebugLayout() const
		{
			const auto& constLayout = GetConstLayout();
			const auto& layoutVector = constLayout.GetVector();

			for (const auto& layoutElement : layoutVector)
			{
				size_t elementOffset = constLayout.GetOffsetOfElement(layoutElement.first.c_str());
				std::cout << "name: " << layoutElement.first << " offsets: " << elementOffset << " size: " << DynamicConstantBuffer::BufferLayout::GetElementSize(layoutElement.second->GetType()) << '\n';
			}
		}
#else
		void DebugLayout() const
		{

		}
#endif

	public:
		void* GetBytes() const;
		size_t GetSize() const;

	public:
		void ResizeBuffer(size_t newSize);

	private:
		size_t m_size = 0;
		size_t latestUseOfAdditionOperator = 0;

		BufferLayout m_layout;

		void* m_pDataBuffer = nullptr;
	};
}

#undef FOR_ALL_TYPES