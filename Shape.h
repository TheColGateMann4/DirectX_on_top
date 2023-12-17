#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "IndexBuffer.h"

class Bindable;

class Shape
{
public:
	Shape() = default;
	Shape(const Shape&) = delete;

public:
	void Draw(GFX& gfx) const noexcept(!IS_DEBUG);
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept = 0;

public:
	template <class T>
	T* GetBindable() noexcept
	{
		for (auto& b : binds)
			if (T* r = dynamic_cast<T*>(b.get()))
				return r;
		return nullptr;
	}

public:
	void AddBindable(std::shared_ptr<Bindable> bindable) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) noexcept(!IS_DEBUG);

private:
	const class IndexBuffer* pIndexBuffer = NULL;
	std::vector<std::shared_ptr<Bindable>> binds;
};