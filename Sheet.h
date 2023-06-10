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
	Sheet(GFX& gfx,	std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		const UINT32 TesselationRatio = 1);

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept;
	VOID Update(FLOAT DeltaTime) noexcept;

private:
	template <class V>
	Mesh<V> GetTesselatedMesh(const UINT32 TesselationRatio);

private:
	// positional
	FLOAT r = 17.0f;
	FLOAT roll = (float)std::_Pi * 0.125f;
	FLOAT pitch = (float)std::_Pi * 2.0f;
	FLOAT yaw = 0.5f;
	FLOAT theta = 0.0f;
	FLOAT phi = (float)std::_Pi * 1.5f;
	FLOAT chi = (float)std::_Pi * 1.0f;

	// speed (delta/s)
	FLOAT droll;
	FLOAT dpitch;
	FLOAT dyaw;
	FLOAT dtheta;
	FLOAT dphi;
	FLOAT dchi;
};

