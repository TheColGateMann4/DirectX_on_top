#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "PointLightModel.h"

class PointLight
{
public:
	PointLight(GFX &gfx, float radius = 0.5f);

public:
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

public:
	void Draw(GFX &gfx) const noexcept(!IS_DEBUG);
	void Bind(GFX &gfx, DirectX::XMMATRIX CameraView_) const noexcept;

private:
	struct PixelConstantStruct { 
		alignas(16) DirectX::XMFLOAT3 position;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;

		float diffuseIntensity;
		float attenuationConst;
		float attenuationLinear;
		float attenuationQuadratic;
	};

	PixelConstantStruct m_pcstruct = {};

	mutable PointLightModel m_model;
	mutable PixelConstantBuffer<PixelConstantStruct> m_pcbuffer;
};

