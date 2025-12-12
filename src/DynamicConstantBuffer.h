#pragma once
#include <vector>
#include <DirectXMath.h>
#include <windows.h>
#include <string>
#include <memory>
#include <iostream>
#include <imgui.h>


#define FOR_ALL_TYPES \
	STATEMENT(Padding) \
	STATEMENT(Int) \
	STATEMENT(Float) \
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
	template<> struct DataTypeMap<DataType::Padding>
	{
		using type = float;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "PD";
	};
	template<> struct DataTypeMap<DataType::Int>
	{
		using type = int;
		static constexpr bool valid = true;
		static constexpr size_t hlslsize = sizeof(type);
		static constexpr const char* code = "IN";
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
		using type = BOOL;
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
			size_t size = 0;
		};
		struct StructData : public AdditionalDataBase
		{
			::std::vector<::std::pair<::std::string, std::shared_ptr<LayoutElement>>> data = {};
		};
	}

	namespace ImguiAdditionalInfo
	{
		struct ImguiInfo
		{
			virtual ~ImguiInfo() = default;
			virtual void aaa() {};
		};
		struct ImguiIntInfo : public ImguiInfo
		{
			int v_min = 0;
			int v_max = 10;
			std::string format = "%d";
			ImGuiSliderFlags flags = ImGuiSliderFlags_None;
		};
		struct ImguiFloatInfo : public ImguiInfo
		{
			float v_min = 0.0f;
			float v_max = 1.0f;
			std::string format = "%.3f";
			ImGuiSliderFlags flags = ImGuiSliderFlags_None;
		};
		struct ImguiColorInfo : public ImguiInfo
		{
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_None;
		};
	}


	class LayoutElement
	{
		friend class BufferData;
		friend class BufferLayout;
	public:

		LayoutElement(DataType elementType, ImguiAdditionalInfo::ImguiInfo* imguiInfo = nullptr);

		LayoutElement();

		~LayoutElement();

	public:
		template<DataType type>
		void Add(const char* elementName) // for DataType::Struct
		{
			assert(this->m_type == Struct && "Cannot add places to not Struct element");

			auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::StructData&>(*m_additionalData);

			additionalData.data.push_back({ elementName, std::make_shared<LayoutElement>(type)});
		}

		template<DataType type>
		void Add(size_t additionaLenght = 1) // for DataType::Array; additionaLenght = number of added space
		{
			assert(this->m_type == Array && "Cannot add space to not Array element");

			auto& additionalData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData&>(*m_additionalData);

			assert(additionalData.type == type && "Assigning element to array with different type");
			assert(additionalData.type != DynamicConstantBuffer::DataType::Empty && "Trying to make an array of empty type");

			additionalData.type = type;
			additionalData.size++;
		}

	public:
		const LayoutElement* operator[] (const char* elementName) noexcept;

	public:
		DataType GetType() const { return m_type; };
		const ImguiAdditionalInfo::ImguiInfo* GetImGuiInfo() const { return m_imguiInfo.get(); };


	private:
		DataType m_type = DataType::Empty;
		::std::unique_ptr<AdditionalElements::AdditionalDataBase> m_additionalData = nullptr;
		::std::shared_ptr<ImguiAdditionalInfo::ImguiInfo> m_imguiInfo;
	};


	class BufferLayout
	{
		friend class BufferData;
	public:
		BufferLayout(const char* bufferIdentificator);

		BufferLayout();

		~BufferLayout();

	public:
		template<DataType type>
		void Add(const char* elementName, ImguiAdditionalInfo::ImguiInfo* imguiInfo = nullptr)
		{
			if(imguiInfo != nullptr)
				m_elements.push_back({ elementName, std::make_shared<LayoutElement>(type, imguiInfo) });
			else
				m_elements.push_back({ elementName, std::make_shared<LayoutElement>(type) });
		}

	public:
		bool isExisting(const char* elementName);

		size_t GetOffsetOfElement(const char* elementName) const;

		size_t GetIndexOfElement(const char* elementName) const;

		const BufferLayout& GetFinished(size_t* newSize = nullptr, std::vector<size_t>* addedSpacesOffsets = nullptr);

		static size_t GetLayoutSize(const BufferLayout& layout);

		void SetArrayAttributes(const char* arrayName, DataType type, size_t size);

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
		size_t GetElementSize(const char* elementName) const;
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
		bool MakeImguiMenu();

	public:
		bool ElementExists(const char* elementName);

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

		template<DataType type>
		DynamicConstantBuffer::DataTypeMap<type>::type* GetArrayDataPointerValue(const char* arrayName, size_t arrayIndex)
		{
			size_t arrayIndexInLayout = m_layout.GetIndexOfElement(arrayName);
			LayoutElement* layoutElement = m_layout.m_elements.at(arrayIndexInLayout).second.get();
			DynamicConstantBuffer::AdditionalElements::ArrayData* arrayData = static_cast<DynamicConstantBuffer::AdditionalElements::ArrayData*>(layoutElement->m_additionalData.get());

			assert(arrayIndex < arrayData->size && "tried to access item out of range");

			size_t bytesOffsetOfElement = m_layout.GetOffsetOfElement(arrayName);
			bytesOffsetOfElement += DynamicConstantBuffer::BufferLayout::GetElementSize(arrayData->type) * arrayIndex;

			assert(bytesOffsetOfElement <= m_size);

			return static_cast<DynamicConstantBuffer::DataTypeMap<type>::type*>(static_cast<void*>(static_cast<char*>(m_pDataBuffer) + bytesOffsetOfElement));
		}

	public:
		void MakeFinished();

	public:
		const BufferLayout& GetLayout() const;

	public:
		template<DataType type>
		void AddLayoutElement(const char* elementName, ImguiAdditionalInfo::ImguiInfo* imguiInfo = nullptr)
		{
			m_layout.Add<type>(elementName, imguiInfo);
			m_size += DynamicConstantBuffer::DataTypeMap<type>::hlslsize;

			void* newBuffer = realloc(m_pDataBuffer, m_size);

			assert(newBuffer != nullptr);

			m_pDataBuffer = newBuffer;
		}

		template<DataType type>
		DynamicConstantBuffer::DataTypeMap<type>::type* AddElement(const char* elementName, ImguiAdditionalInfo::ImguiInfo* imguiInfo = nullptr)
		{
			AddLayoutElement<type>(elementName, imguiInfo);

			return static_cast<DynamicConstantBuffer::DataTypeMap<type>::type*>(static_cast<void*>(static_cast<char*>(m_pDataBuffer) + m_layout.GetOffsetOfElement(elementName)));
		}

	public:
		void Update(const BufferLayout layout);

		void DebugLayout() const;

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