#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "DynamicConstantBuffer.h"

class PointLightModel : public Shape
{
public:
	PointLightModel(GFX& gfx, float radius);

public:
	void UpdateLightColorBuffer(GFX& gfx, DirectX::XMFLOAT3 color);
	DirectX::XMFLOAT3 GetColor() const noexcept { return *m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0"); /*should be named color, but since we make layout by identificator string we don't have normal name*/ };

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	void SetPosition(DirectX::XMFLOAT3 position);

private:
	DynamicConstantBuffer::BufferData m_colorBuffer;

	DirectX::XMFLOAT3 m_position;

	DirectX::XMFLOAT3X3 ModelScale;
};