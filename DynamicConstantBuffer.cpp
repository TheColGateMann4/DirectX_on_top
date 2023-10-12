// #include "DynamicConstantBuffer.h"
// #include <string>
// #include <cmath>
// 
// template<DynamicConstantBuffer::DataType type>
// void DynamicConstantBuffer::BufferLayout::Add(const char* elementName)
// {
// 	DynamicConstantBuffer::LayoutElement element(type);
// 	m_elements.push_back({elementName, std::move(element)});
// }
// 
// const DynamicConstantBuffer::BufferLayout DynamicConstantBuffer::BufferLayout::GetFinished()
// {
// 	size_t bufferLayoutSize = m_elements.size();
// 	
// 	if (bufferLayoutSize == 0)
// 		std::abort(); // make a proper error here
// 	
// 	size_t restOfbufferLayoutSizeInRatio = bufferLayoutSize / 4 - std::floor(bufferLayoutSize / 4);
// 
// 	if (restOfbufferLayoutSizeInRatio != 0) 
// 	{
// 		std::string newElementName = "emptyElement0";
// 		for (size_t i = 0; i < restOfbufferLayoutSizeInRatio * 4; i++)
// 		{
// 			newElementName.at(newElementName.length() - 1) = (char)(i + 48);
// 
// 			m_elements.push_back({ newElementName, {} });
// 		}
// 	}
// 
// 	return std::move(*this);
// }
// 
// 
// DynamicConstantBuffer::BufferLayout DynamicConstantBuffer::BufferLayout::operator=(const DynamicConstantBuffer::BufferLayout& layout) noexcept
// {
// 	//this->m_elements = std::move(layout.m_elements);
// 	return *this;
// }
// 
// 
// DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::BufferLayout::operator[](const char* elementName) noexcept
// {
// 	for (auto& element : m_elements)
// 	{
// 		if (element.first == elementName)
// 			return &element.second;
// 	}
// 
// 	assert(false, std::string(elementName + "element name could not be found in buffer").c_str());
// 	return nullptr;
// }
// 
// 
// const DynamicConstantBuffer::LayoutElement* DynamicConstantBuffer::BufferLayout::operator[](const char* elementName) const noexcept
// {
// 	for (const auto& element : m_elements)
// 	{
// 		if (element.first == elementName)
// 			return &element.second;
// 	}
// 
// 	assert(false, std::string(elementName + "element name could not be found in buffer").c_str());
// 	return nullptr;
// }
// 
// 
// 
// 
// DynamicConstantBuffer::LayoutElement::LayoutElement(DynamicConstantBuffer::DataType elementType)
// {
// 	m_type = elementType;
// }
// 
// DynamicConstantBuffer::LayoutElement::LayoutElement()
// {
// 
// }
// 
// template<DynamicConstantBuffer::DataType type>
// void DynamicConstantBuffer::LayoutElement::Add(const char* elementName)
// {
// 	if (this->m_type != Struct)
// 		std::abort();
// 
// 
// }
// 
// void DynamicConstantBuffer::LayoutElement::Add(size_t additionaLenght)
// {
// 	if (this->m_type != Array)
// 		std::abort();
// 
// 
// }
// 
// DynamicConstantBuffer::BufferData::BufferData(const BufferLayout layout)
// {
// 	m_layout = layout;
// }