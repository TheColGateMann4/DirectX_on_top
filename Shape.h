#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

class Bindable;

class Shape
{
public:
	Shape() = default;
	Shape(const Shape&) = delete;

public:
	void Draw(GFX& gfx) const noexcept(!IS_DEBUG)
	{
		for (auto& b : m_binds)
		{
			b->Bind(gfx);
		}
		gfx.DrawIndexed(m_pIndexBuffer->GetCount());
	}

	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept = 0;

public:
	template <class T>
	T* GetBindable() noexcept
	{
		for (auto& b : m_binds)
			if (T* r = dynamic_cast<T*>(b.get()))
				return r;
		return nullptr;
	}

public:
	void AddBindable(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		if (typeid(*bind) == typeid(IndexBuffer))
		{
			assert("Attempting to bind Index Buffer second time" && m_pIndexBuffer == NULL);
			m_pIndexBuffer = static_cast<IndexBuffer*>(bind.get());
		}

		m_binds.push_back(std::move(bind));
	}

	std::vector<std::shared_ptr<Bindable>>& getAllBindables(const IndexBuffer* &pIndexBuffer) noexcept
	{
		pIndexBuffer = m_pIndexBuffer;
		return m_binds;
	}

private:
	class IndexBuffer* m_pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> m_binds;
};