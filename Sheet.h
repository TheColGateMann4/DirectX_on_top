#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "Time.h"
#include <random>

class Sheet : public Shape
{
public:
	Sheet(GFX& gfx, const UINT32 TesselationRatio = 1, const UINT32 TextureRatio = 1);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	void SpawnControlWindow(GFX& gfx);

private:
	void UpdateConstBuffer(GFX& gfx);
	void Reset();

private:
	SimpleMesh GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio);

private:
	DirectX::XMFLOAT3 m_position = {};
	DirectX::XMFLOAT3 m_rotation = {};
	DirectX::XMFLOAT3 m_scale = {10.0f, 10.0f, 10.0f};

	struct ModelMaterial {
		float specularIntensity = 0.8f;
		float specularPower = 50.0f;
		bool normalMapEnabled = true;
		float padding[1];
	}m_constBuffer;
};

