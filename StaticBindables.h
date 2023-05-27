#pragma once
#include "IndexBuffer.h"
#include "ErrorHandling.h"

template<class T>
class StaticBindables : public Shape
{
protected:
	static BOOL IsStaticInitialized()
	{
		return !m_bindables.empty();
	}

protected:
	VOID AddStaticIndexBufferBind(std::unique_ptr<IndexBuffer> indexBuffer)
	{
		THROW_INTERNAL_ERROR("Attempting to bind Index Buffer second time", pIndexBuffer != NULL);

		StaticBindables<T>::pIndexBuffer = indexBuffer.get();
		m_bindables.push_back(std::move(indexBuffer));
	}
	static VOID AddStaticBind(std::unique_ptr<Bindable> bind)
	{
		THROW_INTERNAL_ERROR("*Must* use AddStaticIndexBufferBind to bind Index Buffer", typeid(bind) == typeid(IndexBuffer));

		m_bindables.push_back(std::move(bind));
	}
	VOID GetIndexBufferFromVector()
	{
		THROW_INTERNAL_ERROR("Attempting to bind Index Buffer second time", pIndexBuffer != NULL);

		for(const auto& index : m_bindables)
			if (const auto buffer = dynamic_cast<IndexBuffer*>(index.get()))
			{
				pIndexBuffer = buffer;
				return;
			}

		THROW_INTERNAL_ERROR("Failed to assign IndexBuffer from Vector", pIndexBuffer == NULL);
	}
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept override
	{
		return m_bindables;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> m_bindables;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> StaticBindables<T>::m_bindables;