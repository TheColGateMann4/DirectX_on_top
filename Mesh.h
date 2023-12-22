#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Shape.h"

class Mesh : public Shape
{
public:
	Mesh(GFX &gfx, std::vector<std::shared_ptr<Bindable>> binds)
	{
		for (auto& bind : binds)
		{
			AddBindable(std::move(bind));
		}

		AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		AddBindable(std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2));
	}

public:
	void Draw(GFX& gfx, DirectX::XMMATRIX transform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&m_transform, transform);
		Shape::Draw(gfx);
	}

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&m_transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 m_transform;
}; 