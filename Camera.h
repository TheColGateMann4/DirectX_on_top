#pragma once
#include "Includes.h"
#include "SceneObject.h"

class Camera : public SceneObject
{
public:
	Camera();

public:
	void SetProjection(DirectX::XMMATRIX projection);

public:
	DirectX::XMMATRIX GetCameraView() const;
	DirectX::XMMATRIX GetProjection() const;

public:
	void Move(const DirectX::XMFLOAT3& moveoffset);
	void Look(const DirectX::XMFLOAT3 lookoffset);

private:
	DirectX::XMMATRIX m_projection = {};

public:
	virtual void MakeTransformPropeties(GFX& gfx) override;

	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	virtual void RenderOnScene() const noexcept(!IS_DEBUG) override;

	void Reset();
	void SpawnControlWindow(class GFX& gfx);

private:
	float WrapAngle(float angle, float value);

	virtual std::string GetName() const override
	{
		return "Camera";
	}

private:
	static constexpr float m_movespeed = 12.0f;
	static constexpr float m_sensivity = 0.008f;
};

