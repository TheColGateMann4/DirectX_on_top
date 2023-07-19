#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "StaticBindables.h"
#include "Time.h"
#include <random>

class CustomShape : public StaticBindables<CustomShape>
{
public:
	CustomShape(GFX& gfx, std::string objpath, DirectX::XMFLOAT3 color, UINT32 objectNumber);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept;
	void Update(FLOAT DeltaTime) noexcept;

public:
	VOID SpawnControlWindow(GFX& gfx) noexcept;
	VOID Reset() noexcept;

private:
	void SyncConstBuffer(GFX& gfx) noexcept(!IS_DEBUG);

private:
	DirectX::XMFLOAT3 m_position = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3 m_rotation = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT3X3 m_scale;

	struct ModelMaterial {
		alignas (16) DirectX::XMFLOAT3 color;
		float specularIntensity;
		float specularPower;
		float padding[2];
	};
	ModelMaterial m_material;
	DirectX::XMFLOAT3 m_defaultColor;
	UINT32 m_objectNumber;
};