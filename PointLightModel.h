#pragma once
#include "Shape.h"
#include "Graphics.h"

class PointLightModel : public Shape
{
public:
	PointLightModel(GFX& gfx, float radius);

public:
	void UpdateLightColorBuffer(GFX& gfx, DirectX::XMFLOAT3 color);
	DirectX::XMFLOAT3 GetColor() { return m_color.color; };

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	void SetPosition(DirectX::XMFLOAT3 position);

private:
	__declspec(align(16))
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
	}m_color;

	DirectX::XMFLOAT3 m_position = {};

	DirectX::XMFLOAT3X3 ModelScale;
};