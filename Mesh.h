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
		binds.push_back(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		for (auto& bind : binds)
		{
			AddBindable(std::move(bind));
		}

		AddBindable(std::make_shared<TransformConstBuffer>(gfx, *this));
	}

public:
	void Draw(GFX& gfx, DirectX::XMMATRIX transform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&m_transform, transform);
		Shape::Draw(gfx, 0.0f);
	}

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&m_transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 m_transform;
}; 