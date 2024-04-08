#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "PointLightModel.h"
#include "DynamicConstantBuffer.h"
#include "SceneObject.h"
#include "Camera.h"

class PointLight : public SceneObject
{
public:
	PointLight(GFX& gfx, float radius = 0.5f);

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;
	virtual void MakeAdditionalPropeties(GFX& gfx, float deltaTime) override;
	void Reset() noexcept;

public:
	virtual void RenderOnScene() const noexcept(!IS_DEBUG) override;
	void Bind(GFX &gfx, DirectX::XMMATRIX CameraView_) const noexcept;

public:
	const char* GetName() const override
	{
		return "PointLight";
	}

private:
	mutable PointLightModel m_model;
	mutable NonCachedBuffer m_pcbuffer;
	mutable DynamicConstantBuffer::BufferData constBufferData;

	Camera m_camera;

private:
	bool enableChroma = false;
	float chromaDeltaTime = 0.02f;
	float lastDeltaTime = 0.0f;
	int colorToDecrement = 0;
	bool incrementingNow = true;
	bool justSwitched = false;
};

