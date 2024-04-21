#pragma once
#include "Includes.h"
#include "SceneObject.h"
#include "CameraIndicator.h"
#include "CameraViewAreaIndicator.h"

class GFX;
class CameraManager;

class Camera : public SceneObject
{
	friend class CameraIndicator;
	friend class CameraViewAreaIndicator;
	friend class ShadowMappingRenderPass;

public:
	Camera(GFX& gfx, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

public:
	DirectX::XMMATRIX GetCameraView() const;
	DirectX::XMMATRIX GetProjection() const;

public:
	void Move(const DirectX::XMFLOAT3& moveoffset);
	void Look(const DirectX::XMFLOAT3 lookoffset, float multiplyBySensivity = false);

public:
	void Reset(GFX& gfx);

	virtual void MakeTransformPropeties(GFX& gfx) override;

	virtual void MakePropeties(GFX& gfx) override;

	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;

	void SetActive(bool active);

private:
	void UpdateProjectionMatrix(GFX& gfx);

	float WrapAngle(float angle, float value);

	virtual const char* GetName() const override
	{
		return "Camera";
	}

	virtual const char* GetNameSpecialStatus() const
	{
		if (m_active)
			return " (Active)";
		return "";
	}

private:
	float m_movespeed = 12.0f;
	float m_sensivity = 0.008f;

	float m_Fov = _Pi / 2;
	float m_AspectRatio;
	float m_NearZ = 0.5f;
	float m_FarZ = 400.0f;

	bool drawFrustum = true;
	bool drawIndicator = true;

	bool m_active;
	size_t m_cameraIndex;

	CameraIndicator m_indicator;
	CameraViewAreaIndicator m_viewIndicator;

	DirectX::XMMATRIX m_projection = {};
};

