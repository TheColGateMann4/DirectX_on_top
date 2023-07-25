#pragma once
#include "Includes.h"

class CameraView
{

public:
	void SetProjection(DirectX::XMMATRIX projection);

public:
	DirectX::XMMATRIX GetCamera();
	DirectX::XMMATRIX GetProjection();

public:
	void Move(DirectX::XMFLOAT3 moveoffset);
	void Look(DirectX::XMFLOAT3 lookoffset);

private:
	DirectX::XMMATRIX m_projection = {};

public:
	void Reset();
	void CreateControlMenu();

private:
	float WrapAngle(float angle, float value);

private:
	DirectX::XMFLOAT3 m_position = { 0.0f, 3.0f, -12.0f };
	DirectX::XMFLOAT3 m_view = {};

	static constexpr float m_movespeed = 12.0f;
	static constexpr float m_sensivity = 0.004f;
};

