#pragma once
#include "Shape.h"
#include "SimpleMesh.h"
#include "Graphics.h"
#include <random>

class Sphere //: public Shape
{
// public:
// 	Sphere(GFX& gfx, std::mt19937& rng,
// 		std::uniform_real_distribution<float>& adist,
// 		std::uniform_real_distribution<float>& ddist,
// 		std::uniform_real_distribution<float>& odist,
// 		std::uniform_real_distribution<float>& rdist,
// 		std::uniform_int_distribution<UINT32>& longdist,
// 		std::uniform_int_distribution<UINT32>& latdist);
// 
// public:
// 	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

public:
	static SimpleMesh GetMesh(UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24)
	{
		assert(verticalDivisions >= 3);
		assert(horizontalDivisions >= 3);

		auto GetFloat3FromVector = [](DirectX::XMVECTOR vec)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, vec);
			return result;
		};

		constexpr float radius = 1.0f;
		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float verticalAngle = (float)std::_Pi / verticalDivisions;
		const float horizontalAngle = 2.0f * (float)std::_Pi / horizontalDivisions;

		DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout().Append(DynamicVertex::VertexLayout::Position3D);
		DynamicVertex::VertexBuffer vertices(std::move(layout));

		for (UINT32 currVerticalDivisions = 1; currVerticalDivisions < verticalDivisions; currVerticalDivisions++)
		{
			const auto verticalBase = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationX(verticalAngle * currVerticalDivisions)
			);
			for (UINT32 currHorizontalDivision = 0; currHorizontalDivision < horizontalDivisions; currHorizontalDivision++)
			{
				DirectX::XMVECTOR position = DirectX::XMVector3Transform(
					verticalBase,
					DirectX::XMMatrixRotationZ(horizontalAngle * currHorizontalDivision)
				);
				vertices.Emplace_Back(GetFloat3FromVector(position));
			}
		}

		// add the cap vertices
		const auto iNorthPole = (UINT32)vertices.GetSize();
		vertices.Emplace_Back(GetFloat3FromVector(base));
		const auto iSouthPole = (UINT32)vertices.GetSize();
		vertices.Emplace_Back(GetFloat3FromVector(DirectX::XMVectorNegate(base)));

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

