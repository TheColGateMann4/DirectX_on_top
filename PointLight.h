#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "PointLightModel.h"
#include "DynamicConstantBuffer.h"

class PointLight
{
public:
	PointLight(GFX &gfx, float radius = 0.5f);

public:
	void SpawnControlWindow(GFX& gfx, float deltaTime) noexcept;
	void Reset() noexcept;

public:
	void Draw(GFX &gfx) const noexcept(!IS_DEBUG);
	void Bind(GFX &gfx, DirectX::XMMATRIX CameraView_) const noexcept;

private:
	mutable PointLightModel m_model;
	mutable NonCachedBuffer m_pcbuffer;
	mutable DynamicConstantBuffer::BufferData constBufferData;

private:
	bool enableChroma = false;
	float chromaDeltaTime = 0.02f;
	float lastDeltaTime = 0.0f;
	int colorToDecrement = 0;
	bool incrementingNow = true;
	bool justSwitched = false;
};

