#pragma once
#include "Includes.h"

class CameraView
{
public:
	CameraView();

public:
	void SetProjection(DirectX::XMMATRIX projection);

public:
	DirectX::XMMATRIX GetCamera() const;
	DirectX::XMMATRIX GetProjection() const;

public:
	void Move(DirectX::XMFLOAT3 moveoffset);
	void Look(DirectX::XMFLOAT3 lookoffset);

private:
	DirectX::XMMATRIX m_projection = {};

public:
	void Reset();
	std::string SpawnControlWindow();

private:
	float WrapAngle(float angle, float value);

private:
	std::vector<std::string> filterOptions = { "Normal", "NegativeColors", "Blur" };
	std::string currentFilter;
	DirectX::XMFLOAT3 m_position = { -37.5f, 7.0f, 1.0f };
	DirectX::XMFLOAT3 m_view = { 0.5f * _Pi, 0.0f, 0.0f};

	static constexpr float m_movespeed = 12.0f;
	static constexpr float m_sensivity = 0.004f;
};

