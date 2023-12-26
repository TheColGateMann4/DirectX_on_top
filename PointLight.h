#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "PointLightModel.h"
#include "DynamicConstantBuffer.h"
#include "SceneObject.h"

class PointLight : public SceneObject
{
public:
	PointLight(GFX &gfx, float radius = 0.5f);

public:
	void MakePropeties(GFX& gfx, float deltaTime) override;
	void Reset() noexcept;

public:
	virtual void RenderOnScene(RenderQueue& renderQueue) const noexcept(!IS_DEBUG) override;
	void Bind(GFX &gfx, DirectX::XMMATRIX CameraView_) const noexcept;

public:
	std::string GetName() const override
	{
		return "PointLight";
	}

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

