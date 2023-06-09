#pragma once
#include "Shape.h"
#include "Mesh.h"
#include "Graphics.h"
#include "StaticBindables.h"
#include <random>

class Sphere : public StaticBindables<Sphere>
{
public:
	Sphere(GFX& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_int_distribution<UINT32>& longdist,
		std::uniform_int_distribution<UINT32>& latdist);

	VOID Update(FLOAT DeltaTime) noexcept override;
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

private:
	template <class T>
	static Mesh<T> GetMesh(UINT32 latDiv = 12, UINT32 longDiv = 24);
private:
	// positional
	FLOAT r;
	FLOAT roll = 0.0f;
	FLOAT pitch = 0.0f;
	FLOAT yaw = 0.0f;
	FLOAT theta;
	FLOAT phi;
	FLOAT chi;
	// speed (delta/s)
	FLOAT droll;
	FLOAT dpitch;
	FLOAT dyaw;
	FLOAT dtheta;
	FLOAT dphi;
	FLOAT dchi;
};

