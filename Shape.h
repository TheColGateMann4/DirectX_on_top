#pragma once
#include "Includes.h"
#include "Graphics.h"

class Bindable;

class Shape
{
	template<class T>
	friend class StaticBindables;

public:
	Shape() = default;
	Shape(const Shape&) = delete;
	virtual ~Shape() = default;

public:
	VOID Draw(GFX& gfx, float time) const;
	virtual VOID Update(float DeltaTime) noexcept = 0;
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept = 0;

public:
	VOID AddBindable(std::unique_ptr<Bindable> bindable);
	VOID AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept;

private:
	const class IndexBuffer* pIndexBuffer = NULL;
	std::vector<std::unique_ptr<Bindable>> binds;

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBindables() const noexcept = 0;
};