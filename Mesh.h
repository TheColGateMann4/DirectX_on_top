#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Shape.h"

class Mesh : public Shape
{
public:
	void Render(RenderQueue& renderQueue, DirectX::XMMATRIX transform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&m_transform, transform);
		Shape::Render(renderQueue);
	}

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&m_transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 m_transform = {};
}; 