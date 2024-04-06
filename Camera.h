#pragma once
#include "Includes.h"
#include "SceneObject.h"
#include "CameraIndicator.h"
#include "CameraViewAreaIndicator.h"

class GFX;
class CameraManager;

class Camera : public SceneObject
{
	friend class CameraManager;
	friend class CameraIndicator;
	friend class CameraViewAreaIndicator;

public:
	Camera(GFX& gfx, CameraManager* cameraManager);

public:
	DirectX::XMMATRIX GetCameraView() const;
	DirectX::XMMATRIX GetProjection() const;

public:
	void Move(const DirectX::XMFLOAT3& moveoffset);
	void Look(const DirectX::XMFLOAT3 lookoffset);

public:
	void Reset(GFX& gfx);

	virtual void MakeTransformPropeties(GFX& gfx) override;

	virtual void MakePropeties(GFX& gfx) override;

	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	virtual void RenderOnScene() const noexcept(!IS_DEBUG) override;

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

	bool m_active;
	bool m_selected = false;
	size_t m_cameraIndex;

	CameraManager* m_cameraManager;

	CameraIndicator m_indicator;
	CameraViewAreaIndicator m_viewIndicator;

	DirectX::XMMATRIX m_projection = {};
};

