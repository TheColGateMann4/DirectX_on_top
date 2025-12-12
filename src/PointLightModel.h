#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "DynamicConstantBuffer.h"
#include "mesh.h"

class PointLight;

class PointLightModel : public Shape
{
public:
	PointLightModel(GFX& gfx, PointLight* parent, float radius);

public:
	void UpdateLightColorBuffer(GFX& gfx, const DirectX::XMFLOAT3& color);
	DirectX::XMFLOAT3 GetColor() const noexcept { return *m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0"); /*should be named color, but since we make layout by identificator string we don't have normal name*/ };

	DirectX::XMMATRIX GetTranformMatrix() const noexcept;

private:
	DynamicConstantBuffer::BufferData m_colorBuffer;

	PointLight* m_parent;

	DirectX::XMFLOAT3 m_scale = {1.0f, 1.0f, 1.0f};
};