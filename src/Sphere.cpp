#include "Sphere.h"
#include "ErrorMacros.h"

SimpleMesh Sphere::GetMesh(float radius, UINT32 verticalDivisions, UINT32 horizontalDivisions)
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
SimpleMesh Sphere::GetMeshWithNormalsAndTextureMapping(float radius, UINT32 verticalDivisions, UINT32 horizontalDivisions)
{
	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout()
		.Append(DynamicVertex::VertexLayout::Position3D)
		.Append(DynamicVertex::VertexLayout::Normal)
		.Append(DynamicVertex::VertexLayout::Tangent)
		.Append(DynamicVertex::VertexLayout::Bitangent)
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
			vertices.Emplace_Back(DirectX::XMFLOAT3(x * radius, y * radius, z * radius), DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(u, v));
		}
		for (int i = 0; i <= circCnt_2; i++)
		{
			float u = (float)i / (float)circCnt_2;
			float angle = _Pi * u + _Pi;
			float x = cos(angle) * cosUp;
			float y = sin(angle) * cosUp;
			vertices.Emplace_Back(DirectX::XMFLOAT3(x * radius, y * radius, z * radius), DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(u, v));
		}
	}
	std::vector<UINT32> indices;
	size_t numFaces = 0;
	{
		auto AddFace3 = [&indices, &numFaces](int first, int second, int third) mutable
			{
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(third);
				numFaces++;
			};

		auto AddFace4 = [&indices, &numFaces](int first, int second, int third, int fourth) mutable
			{
				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(third);
				numFaces++;
				indices.push_back(third);
				indices.push_back(fourth);
				indices.push_back(first);
				numFaces++;
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

	return { std::move(vertices), std::move(indices), numFaces };
}




DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3 first, const DirectX::XMFLOAT3 second)
{
	DirectX::XMFLOAT3 result = {};

	result.x = second.x - first.x;
	result.y = second.y - first.y;
	result.z = second.z - first.z;

	return result;
}


DirectX::XMFLOAT3 operator/=(const DirectX::XMFLOAT3 first, const float second)
{
	DirectX::XMFLOAT3 result = {};

	result.x = first.x / second;
	result.y = first.y / second;
	result.z = first.z / second;

	return result;
}


DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3 first, const DirectX::XMFLOAT3 second)
{
	DirectX::XMFLOAT3 result = {};

	result.x = second.x * first.x;
	result.y = second.y * first.y;
	result.z = second.z * first.z;

	return result;
}


DirectX::XMFLOAT3 operator^(const DirectX::XMFLOAT3 first, const DirectX::XMFLOAT3 second)
{
	DirectX::XMFLOAT3 result = {};

	result.x = first.y * second.z - first.z * second.y;
	result.y = first.z * second.x - first.x * second.z;
	result.z = first.x * second.y - first.y * second.x;

	return result;
}



#define GetFaceAttribute(elementName, componentName) face.elementName.Attribute<DynamicVertex::VertexLayout::VertexComponent::componentName>()

// Tangent Calculation Logic is taken from Assimp model parser. File: CalcTangentsProcess.cpp ProcessMesh()
// https://github.com/assimp/assimp/blob/master/code/PostProcessing/CalcTangentsProcess.cpp
void Sphere::CalculateTangents(SimpleMesh& mesh)
{
	auto is_special_float = [](float number)
		{
			return isnan(number) || isinf(number);
		};

	auto NormalizeSafe = [](DirectX::XMFLOAT3 vec)
		{
			float len = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
			if (len > static_cast<float>(0)) {
				vec /= len;
			}
			return vec;
		};

	for (unsigned int a = 0; a < mesh.GetNumberOfFaces(); a++)
	{
		SimpleMesh::Face face = mesh.GetFace(a);

		// position differences p1->p2 and p1->p3
		DirectX::XMFLOAT3 v = GetFaceAttribute(vSecond, Position3D) - GetFaceAttribute(vFirst, Position3D), w = GetFaceAttribute(vThird, Position3D) - GetFaceAttribute(vFirst, Position3D);

		// texture offset p1->p2 and p1->p3
		float sx = GetFaceAttribute(vSecond, Texture2D).x - GetFaceAttribute(vFirst, Texture2D).x, sy = GetFaceAttribute(vSecond, Texture2D).y - GetFaceAttribute(vFirst, Texture2D).y;
		float tx = GetFaceAttribute(vThird, Texture2D).x - GetFaceAttribute(vFirst, Texture2D).x, ty = GetFaceAttribute(vThird, Texture2D).y - GetFaceAttribute(vFirst, Texture2D).y;
		float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

		// when t1, t2, t3 in same position in UV space, just use default UV direction.
		if (sx * ty == sy * tx) {
			sx = 0.0;
			sy = 1.0;
			tx = 1.0;
			ty = 0.0;
		}

		// tangent points in the direction where to positive X axis of the texture coord's would point in model space
		// bitangent's points along the positive Y axis of the texture coord's, respectively
		DirectX::XMFLOAT3 tangent, bitangent;
		tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
		tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
		tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
		bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
		bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
		bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;


		auto GetNormalAtIndex = [&face](size_t index)
			{
				if (index == 0)
					return GetFaceAttribute(vFirst, Normal);
				else if (index == 1)
					return GetFaceAttribute(vSecond, Normal);
				else if (index == 2)
					return GetFaceAttribute(vThird, Normal);

				THROW_INTERNAL_ERROR("We got out of our vertice work range. Tried to process more than three.");
			};


		// store for every vertex of that face
		for (unsigned int b = 0; b < 3; ++b)
		{
			size_t p = 0;

			if (b == 0)
				p = face.iFirst;
			else if (b == 1)
				p = face.iSecond;
			else if (b == 2)
				p = face.iThird;

			// project tangent and bitangent into the plane formed by the vertex' normal
			DirectX::XMFLOAT3 localTangent = tangent - GetNormalAtIndex(b) * (tangent * GetNormalAtIndex(b));
			DirectX::XMFLOAT3 localBitangent = bitangent - GetNormalAtIndex(b) * (bitangent * GetNormalAtIndex(b));

			NormalizeSafe(localTangent);
			NormalizeSafe(localBitangent);

			// reconstruct tangent/bitangent according to normal and bitangent/tangent when it's infinite or NaN.
			bool invalid_tangent = is_special_float(localTangent.x) || is_special_float(localTangent.y) || is_special_float(localTangent.z);
			bool invalid_bitangent = is_special_float(localBitangent.x) || is_special_float(localBitangent.y) || is_special_float(localBitangent.z);
			if (invalid_tangent != invalid_bitangent) {
				if (invalid_tangent) {
					localTangent = GetNormalAtIndex(b) ^ localBitangent;
					NormalizeSafe(localTangent);
				}
				else {
					localBitangent = localTangent ^ GetNormalAtIndex(b);
					NormalizeSafe(localBitangent);
				}
			}

			// and write it into the mesh.
			mesh.m_vertices[p].SetAttributeByIndex<DirectX::XMFLOAT3&>(mesh.m_vertices.GetLayout().GetIndexOfElement(DynamicVertex::VertexLayout::Tangent), localTangent);
			mesh.m_vertices[p].SetAttributeByIndex<DirectX::XMFLOAT3&>(mesh.m_vertices.GetLayout().GetIndexOfElement(DynamicVertex::VertexLayout::Bitangent), localBitangent);
		}
	}
}

#undef GetFaceAttribute