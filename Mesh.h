#pragma once
#include "Includes.h"
#include "ErrorHandling.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "Shape.h"

class Mesh : public Shape
{
public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&m_transform);
	}

	void SetTranformMatrix(DirectX::XMMATRIX transform) noexcept
	{
		return DirectX::XMStoreFloat4x4(&m_transform, transform);
	}

private:
	mutable DirectX::XMFLOAT4X4 m_transform = {};
}; 