#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "StaticBindables.h"

class PointLightModel : public StaticBindables<PointLightModel>
{
public:
	PointLightModel(GFX& gfx, float radius);

public:
	VOID Update(FLOAT DeltaTime) noexcept override;
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	VOID SetPosition(DirectX::XMFLOAT3 position);

private:
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3X3 ModelScale;
};