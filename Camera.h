#pragma once
#include "Includes.h"
#include "SceneObject.h"
#include "CameraIndicator.h"
#include "CameraViewAreaIndicator.h"

class GFX;

struct CameraSettings
{
	float m_Fov;
	float m_AspectRatio;
	float m_NearZ;
	float m_FarZ;
};

class Camera : public SceneObject
{
public:
	Camera(GFX& gfx, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f }, float aspectRatio = 0.0f);

	virtual ~Camera() = default;

public:
	DirectX::XMMATRIX GetCameraView() const;
	DirectX::XMMATRIX GetProjection() const;
	DirectX::XMMATRIX GetProjectionView() const; // making this function to keep directX matrix multiplication functions inside camera.cpp

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

	void SetUpVector(DirectX::XMFLOAT3 upVector);

	void GetCameraSettings(CameraSettings* cameraSettings) const;

	const DirectX::XMFLOAT3* GetFrustumBuffer() const;

	UINT32 GetFrustumBufferByteSize() const;

	void SetCameraManagerLinkage(CameraManager* cameraManager);

private:
	float WrapAngle(float angle, float value);

protected:
	virtual void OnHierarchyFocus() override;

	virtual void OnHierarchyUnfocus() override;

protected:
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

protected:
	void UpdateProjectionMatrix(GFX& gfx);

protected:
	float m_movespeed = 12.0f;
	float m_sensivity = 0.008f;

	float m_Fov = _Pi / 2;
	float m_AspectRatio;
	float m_NearZ = 0.5f;
	float m_FarZ = 400.0f;

private:
	bool drawFrustum = true;
	bool drawIndicator = true;

	bool m_active;
	size_t m_cameraIndex;

	CameraIndicator m_indicator;
	CameraViewAreaIndicator m_viewIndicator;

	DirectX::XMMATRIX m_projection = {};

	DirectX::XMVECTOR m_upVector;

	CameraManager* m_cameraManager;
};

