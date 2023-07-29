#pragma once
#include "Shape.h"
#include "Graphics.h"

class PointLightModel : public Shape
{
public:
	PointLightModel(GFX& gfx, float radius);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	VOID SetPosition(DirectX::XMFLOAT3 position);

private:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3X3 ModelScale;
};