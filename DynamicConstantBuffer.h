// #pragma once
// #include <vector>
// #include <DirectXMath.h>
// #include <windows.h>
// #include <string>
// 
// #define FOR_ALL_TYPES STATEMENT(Float) \
// 	STATEMENT(Float2) \
// 	STATEMENT(Float3) \
// 	STATEMENT(Float4) \
// 	STATEMENT(Matrix) \
// 	STATEMENT(Bool)
// 
// 
// namespace DynamicConstantBuffer
// {
// 
// 	enum DataType
// 	{
// 		Empty,
// 
// #define STATEMENT(enum_element) enum_element,
// 		FOR_ALL_TYPES
// #undef STATEMENT
// 
// 		Struct,
// 		Array
// 	};
// 
// 
// 
// 	template<DataType type>
// 	struct DataTypeMap // Empty, Array and Struct will be invalid
// 	{
// 		static constexpr bool valid = false;
// 	};
// 	template<> struct DataTypeMap<DataType::Float>
// 	{
// 		using e_type = float;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = sizeof(e_type);
// 		static constexpr const char* code = "F1";
// 	};
// 	template<> struct DataTypeMap<DataType::Float2>
// 	{
// 		using e_type = DirectX::XMFLOAT2;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = sizeof(e_type);
// 		static constexpr const char* code = "F2";
// 	};
// 	template<> struct DataTypeMap<DataType::Float3>
// 	{
// 		using e_type = DirectX::XMFLOAT3;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = sizeof(e_type);
// 		static constexpr const char* code = "F3";
// 	};
// 	template<> struct DataTypeMap<DataType::Float4>
// 	{
// 		using e_type = DirectX::XMFLOAT4;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = sizeof(e_type);
// 		static constexpr const char* code = "F4";
// 	};
// 	template<> struct DataTypeMap<DataType::Matrix>
// 	{
// 		using e_type = DirectX::XMMATRIX;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = sizeof(e_type);
// 		static constexpr const char* code = "MA";
// 	};
// 	template<> struct DataTypeMap<DataType::Bool>
// 	{
// 		using e_type = bool;
// 		static constexpr bool valid = true;
// 		static constexpr size_t hlslsize = 4;
// 		static constexpr const char* code = "BO";
// 	};
// 
// 
// 	
// #define STATEMENT(enum_element) static_assert(DataTypeMap<enum_element>::valid, "missing definition for " #enum_element);
// 	FOR_ALL_TYPES
// #undef STATEMENT
// 	
// 	
// 	
// 	
// 	class LayoutElement;
// 	class BufferData;
// 	struct AdditionalElementData;
// 
// 
// 	class BufferLayout
// 	{
// 	public:
// 		template<DataType type>
// 		void Add(const char* elementName);
// 
// 	public:
// 		const BufferLayout GetFinished();
// 
// 	public:
// 		BufferLayout operator= (const BufferLayout& layout) noexcept;
// 
// 		LayoutElement* operator[] (const char* elementName) noexcept;
//  		const LayoutElement* operator[] (const char* elementName) const noexcept;
// 
// 	private:
// 		std::vector<std::pair<std::string, LayoutElement>> m_elements = {};
// 	};
// 
// 	class LayoutElement
// 	{
// 	public:
// 		LayoutElement(DataType elementType);
// 
// 		LayoutElement();
// 
// 	public:
// 		template<DataType type>
// 		void Add(const char* elementName); // for DataType::Struct
// 
// 		void Add(size_t additionaLenght = 1); // if user decides to widen the DataType::Array; additionaLenght = number of added space
// 
// 	public:
// 		DataType GetType() const { return m_type; };
// 
// 	private:
// 		DataType m_type = DataType::Empty;
// 		AdditionalElementData m_additionalData = {};
// 	};
// 
// 
// 
// 	struct AdditionalElementData
// 	{
// 		struct ArrayData
// 		{
// 			LayoutElement data;
// 			size_t size;
// 		};
// 		struct StructData
// 		{
// 			std::vector<std::pair<std::string, LayoutElement>> data;
// 		};
// 	};
// 
// 
// 	class BufferData
// 	{
// 	public:
// 		BufferData(const BufferLayout layout);
// 
// 
// 	private:
// 		BufferLayout m_layout;
// 	};
// }
// 
// #undef FOR_ALL_TYPES