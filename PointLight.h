#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "PointLightModel.h"
#include "DynamicConstantBuffer.h"
#include "SceneObject.h"

class ShadowCamera;

class PointLight : public SceneObject
{
public:
	PointLight(GFX& gfx, float radius = 0.5f, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

public:
	ShadowCamera* GetShadowCamera();
	
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	virtual void Update(GFX& gfx, float deltaTime) override;

	virtual void MakeAdditionalPropeties(GFX& gfx) override;

	void Reset() noexcept;

public:
	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;
	void Bind(GFX &gfx, DirectX::XMMATRIX CameraView_) const noexcept;

protected:
	const char* GetName() const override
	{
		return "PointLight";
	}

private:
	mutable PointLightModel m_model;
	mutable NonCachedBuffer m_pcbuffer;
	mutable DynamicConstantBuffer::BufferData constBufferData;
	ShadowCamera* m_cameraChild;

private:
	bool enableChroma = false;
	float chromaDeltaTime = 0.02f;
	float accumulatedDeltaTime = 0.0f;
	int colorToDecrement = 0;
	bool incrementingNow = true;
	bool justSwitched = false;
};

