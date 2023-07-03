#pragma once
#include "Includes.h"

class CameraView
{

public:
	VOID SetProjection(DirectX::XMMATRIX projection);

public:
	DirectX::XMMATRIX GetCamera();
	DirectX::XMMATRIX GetProjection();

private:
	DirectX::XMMATRIX m_projection = {};

public:
	VOID Reset();
	VOID CreateControlMenu();

private:
	FLOAT r = 20.0f;
	FLOAT theta = 0.0f;
	FLOAT phi = 0.0f;
	FLOAT chi = 0.0f;
	FLOAT roll = 0.0f;
	FLOAT pitch = 0.0f;
	FLOAT yaw = 0.0f;
// 	bool toggle;
// 
// public:
// 	int curritem = 0;
// 	const char* models[6] = { "Cow", "Pumpkin", "Teapot", "Teddybear" };
};

