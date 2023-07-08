#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "StaticBindables.h"
#include "Mesh.h"
#include "Time.h"
#include <random>

class CustomShape : public StaticBindables<CustomShape>
{
public:
	CustomShape(GFX& gfx, std::wstring objpath, std::wstring texturePath);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept;
	VOID Update(FLOAT DeltaTime) noexcept;

private:
	template <class V>
	Mesh<V> GetCustomMesh(std::wstring& objpath);

private:
	// positional

	FLOAT r = 0.0f;
	FLOAT roll = 0.0f;
	FLOAT pitch = 0.0f;
	FLOAT yaw = 0.0f;
	FLOAT theta = 0.0f;
	FLOAT phi = 0.0f;
	FLOAT chi = 0.0f;

	// speed (delta/s)
	FLOAT droll; //= 3.0f;
	FLOAT dpitch;
	FLOAT dyaw;
	FLOAT dtheta;
	FLOAT dphi;
	FLOAT dchi;
};