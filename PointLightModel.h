#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "DynamicConstantBuffer.h"
#include "mesh.h"

class PointLightModel
{
public:
	PointLightModel(GFX& gfx, float radius);

public:
	void UpdateLightColorBuffer(GFX& gfx, DirectX::XMFLOAT3 color);
	DirectX::XMFLOAT3 GetColor() const noexcept { return *m_colorBuffer.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("element0"); /*should be named color, but since we make layout by identificator string we don't have normal name*/ };

public:
	void Draw(GFX& gfx) const noexcept(!IS_DEBUG)
	{
		const auto finalTransform =
			(
				DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
				DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z)
			);

		m_mesh->Draw(gfx, finalTransform);
	}

	DirectX::XMMATRIX GetTranformMatrix() const noexcept;

public:
	void SetPosition(DirectX::XMFLOAT3 position);

private:
	DynamicConstantBuffer::BufferData m_colorBuffer;

	DirectX::XMFLOAT3 m_position;

	DirectX::XMFLOAT3 m_scale = {1.0f, 1.0f, 1.0f};

private:
	std::unique_ptr<Mesh> m_mesh;
};