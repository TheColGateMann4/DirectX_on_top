#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Shape.h"

class Mesh : public StaticBindables<Mesh>
{
public:
	Mesh(GFX &gfx, std::vector<std::unique_ptr<Bindable>> binds)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}

		for (auto& bind : binds)
		{
			if (auto pIndexBuffer = dynamic_cast<IndexBuffer*>(bind.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>(pIndexBuffer));
				bind.release();
			}
			else
			{
				AddBindable(std::move(bind));
			}
		}

		AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));
	}

public:
	void Update(float DeltaTime) noexcept override
	{

	}

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