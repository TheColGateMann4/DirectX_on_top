#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "StaticBindables.h"
#include "Mesh.h"
#include "Time.h"
#include <random>

class Sheet : public StaticBindables<Sheet>
{
public:
	Sheet(GFX& gfx,	const UINT32 TesselationRatio = 1, const UINT32 TextureRatio = 1);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept;
	VOID Update(FLOAT DeltaTime) noexcept;

private:
	template <class V>
	Mesh<V> GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio);

private:
	// positional

	FLOAT r = 17.0f;
	FLOAT roll = (float)std::_Pi * 0.5f; //(float)std::_Pi * 0.125f
	FLOAT pitch = (float)std::_Pi * 1.5f; //(float)std::_Pi * 2.0f
	FLOAT yaw = 0.0f; //0.5f;
	FLOAT theta = 0.0f;
	FLOAT phi = (float)std::_Pi * 1.5f;
	FLOAT chi = (float)std::_Pi * 1.0f;

	// speed (delta/s)
	FLOAT droll; //= 3.0f;
	FLOAT dpitch;
	FLOAT dyaw;
	FLOAT dtheta;
	FLOAT dphi;
	FLOAT dchi;
};

