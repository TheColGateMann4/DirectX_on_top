#pragma once
#include "Shape.h"
#include "SimpleMesh.h"
#include "Graphics.h"
#include <random>

class Sphere //: public Shape
{

public:
	static SimpleMesh GetMesh(float radius = 1.0f, UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24)
	{
		assert(verticalDivisions >= 3);
		assert(horizontalDivisions >= 3);

		auto GetFloat3FromVector = [](DirectX::XMVECTOR vec)
		{
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, vec);
			return result;
		};

		const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float verticalAngle = (float)_Pi / verticalDivisions;
		const float horizontalAngle = 2.0f * (float)_Pi / horizontalDivisions;

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

	// Sphere generating algorithm taken from StackOverflow, made by Rabbid76
	//https://stackoverflow.com/questions/45482988/generating-spheres-vertices-indices-and-normals
	static SimpleMesh GetMeshWithNormalsAndTextureMapping(float radius = 1.0f, UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24)
	{
		DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Texture2D);

		DynamicVertex::VertexBuffer vertices(std::move(layout));

		int circCnt = (int)(horizontalDivisions + 0.5f);
		if (circCnt < 4) circCnt = 4;
		int circCnt_2 = circCnt / 2;
		int layerCount = (int)(horizontalDivisions + 0.5f);
		if (layerCount < 2) layerCount = 2;

		for (int tbInx = 0; tbInx <= layerCount; tbInx++)
		{
			float v = (1.0 - (float)tbInx / layerCount);
			float heightFac = sin((1.0 - 2.0 * tbInx / layerCount) * _Pi / 2.0);
			float cosUp = sqrt(1.0 - heightFac * heightFac);
			float z = heightFac;
			for (int i = 0; i <= circCnt_2; i++)
			{
				float u = (float)i / (float)circCnt_2;
				float angle = _Pi * u;
				float x = cos(angle) * cosUp;
				float y = sin(angle) * cosUp;
				vertices.Emplace_Back(DirectX::XMFLOAT3(x * radius, y * radius, z * radius), DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT2(u, v));
			}
			for (int i = 0; i <= circCnt_2; i++)
			{
				float u = (float)i / (float)circCnt_2;
				float angle = _Pi * u + _Pi;
				float x = cos(angle) * cosUp;
				float y = sin(angle) * cosUp;
				vertices.Emplace_Back(DirectX::XMFLOAT3(x * radius, y * radius, z * radius), DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT2(u, v));
			}
		}
		std::vector<UINT32> indices;

		{
			auto AddFace3 = [&indices](int first, int second, int third) mutable
				{
					indices.push_back(first);
					indices.push_back(second);
					indices.push_back(third);
				};

			auto AddFace4 = [&indices](int first, int second, int third, int fourth) mutable
				{
					indices.push_back(first);
					indices.push_back(second);
					indices.push_back(third);
					indices.push_back(third);
					indices.push_back(fourth);
					indices.push_back(first);
				};

			int circSize_2;
			int circSize;
			//Bottom cap
			{
				circSize_2 = circCnt_2 + 1;
				circSize = circSize_2 * 2;
				for (int i = 0; i < circCnt_2; i++)
					AddFace3(circSize + i, circSize + i + 1, i);
				for (int i = circCnt_2 + 1; i < 2 * circCnt_2 + 1; i++)
					AddFace3(circSize + i, circSize + i + 1, i);
			}

			//Discs
			{
				for (int tbInx = 1; tbInx < layerCount - 1; tbInx++)
				{
					int ringStart = tbInx * circSize;
					int nextRingStart = (tbInx + 1) * circSize;
					for (int i = 0; i < circCnt_2; i++)
						AddFace4(ringStart + i, nextRingStart + i, nextRingStart + i + 1, ringStart + i + 1);
					ringStart += circSize_2;
					nextRingStart += circSize_2;
					for (int i = 0; i < circCnt_2; i++)
						AddFace4(ringStart + i, nextRingStart + i, nextRingStart + i + 1, ringStart + i + 1);
				}
			}

			//Top cap
			{
				int start = (layerCount - 1) * circSize;
				for (int i = 0; i < circCnt_2; i++)
					AddFace3(start + i + 1, start + i, start + i + circSize);
				for (int i = circCnt_2 + 1; i < 2 * circCnt_2 + 1; i++)
					AddFace3(start + i + 1, start + i, start + i + circSize);
			}
		}

		return {std::move(vertices), std::move(indices)};
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

