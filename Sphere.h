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

public:
	VOID Update(FLOAT DeltaTime) noexcept override;
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	template <class T>
	static Mesh<T> GetMesh(UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24)
	{
		assert(verticalDivisions >= 3);
		assert(horizontalDivisions >= 3);

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float verticalAngle = (float)std::_Pi / verticalDivisions;
		const float horizontalAngle = 2.0f * (float)std::_Pi / horizontalDivisions;

		std::vector<T> vertices;
		for (UINT32 currVerticalDivisions = 1; currVerticalDivisions < verticalDivisions; currVerticalDivisions++)
		{
			const auto verticalBase = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationX(verticalAngle * currVerticalDivisions)
			);
			for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions; currHorizontalDivision++)
			{
				vertices.emplace_back();
				auto v = DirectX::XMVector3Transform(
					verticalBase,
					DirectX::XMMatrixRotationZ(horizontalAngle * currHorizontalDivision)
				);
				DirectX::XMStoreFloat3(&vertices.back().position, v);
			}
		}

		// add the cap vertices
		const auto iNorthPole = (UINT32)vertices.size();
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().position, base);
		const auto iSouthPole = (UINT32)vertices.size();
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().position, DirectX::XMVectorNegate(base));

		const auto ResolveIndex = [horizontalDivisions](UINT32 currVerticalDivision, UINT32 currHorizontalDivision)
		{ return currVerticalDivision * horizontalDivisions + currHorizontalDivision; };

		std::vector<UINT32> indices;
		for (UINT32 currVerticalDivision = 0; currVerticalDivision < verticalDivisions - 2; currVerticalDivision++)
		{
			for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions - 1; currHorizontalDivision++)
			{
				indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision));
				indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision));
				indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision + 1));
				indices.push_back(ResolveIndex(currVerticalDivision, currHorizontalDivision + 1));
				indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision));
				indices.push_back(ResolveIndex(currVerticalDivision + 1, currHorizontalDivision + 1));
			}
			// wrap band
			indices.push_back(ResolveIndex(currVerticalDivision, horizontalDivisions - 1));
			indices.push_back(ResolveIndex(currVerticalDivision + 1, horizontalDivisions - 1));
			indices.push_back(ResolveIndex(currVerticalDivision, 0));
			indices.push_back(ResolveIndex(currVerticalDivision, 0));
			indices.push_back(ResolveIndex(currVerticalDivision + 1, horizontalDivisions - 1));
			indices.push_back(ResolveIndex(currVerticalDivision + 1, 0));
		}

		// cap fans
		for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions - 1; currHorizontalDivision++)
		{
			// north
			indices.push_back(iNorthPole);
			indices.push_back(ResolveIndex(0, currHorizontalDivision));
			indices.push_back(ResolveIndex(0, currHorizontalDivision + 1));
			// south
			indices.push_back(ResolveIndex(verticalDivisions - 2, currHorizontalDivision + 1));
			indices.push_back(ResolveIndex(verticalDivisions - 2, currHorizontalDivision));
			indices.push_back(iSouthPole);
		}
		// wrap triangles
		// north
		indices.push_back(iNorthPole);
		indices.push_back(ResolveIndex(0, horizontalDivisions - 1));
		indices.push_back(ResolveIndex(0, 0));
		// south
		indices.push_back(ResolveIndex(verticalDivisions - 2, 0));
		indices.push_back(ResolveIndex(verticalDivisions - 2, horizontalDivisions - 1));
		indices.push_back(iSouthPole);

		return { std::move(vertices),std::move(indices) };
	}

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

