#include "Cube.h"
#include "BindableClassesMacro.h"
#include "Vertex.h"
#include "DynamicConstantBuffer.h"
#include "imgui/imgui.h"

#include "RenderTechnique.h"
#include "RenderPass.h"
#include "RenderStep.h"

Cube::Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, DirectX::XMFLOAT3 startingPosition)
	:
	SceneObject(startingPosition)
{
	SetShape(this);

	SimpleMesh CubeModel = GetUnwrappedMesh(scale, true);


	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), CubeModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), CubeModel.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0}, {TargetPixelShader, 2} });
	
	{
		RenderTechnique shadowTechnique("shadow");

		{
			RenderStep shadowStep("shadowMappingPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Shadow.cso");

			shadowStep.AddBindable(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(std::move(pVertexShader));

			shadowTechnique.AddRenderStep(shadowStep);
		}

		AddRenderTechnique(shadowTechnique);
	}

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Cube.cso");

			DynamicConstantBuffer::BufferLayout layout;

			DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
			floatInfo.v_min = 0.001f;
			floatInfo.v_max = 150.0f;

			layout.Add<DynamicConstantBuffer::DataType::Float>("specularIntensity", &floatInfo);
			layout.Add<DynamicConstantBuffer::DataType::Float>("specularPower", &floatInfo);
			layout.Add<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled");

			floatInfo.v_min = 0.1f;
			floatInfo.v_max = 5.0f;
			floatInfo.format = "%.2f";
			layout.Add<DynamicConstantBuffer::DataType::Float>("normalMapWeight", &floatInfo);

			DynamicConstantBuffer::BufferData bufferData(std::move(layout));
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity") = 2.0f;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower") = 150.0f;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled") = (size_t)1;
			*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("normalMapWeight") = 1.0f;


			normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Phong_Texture_Normals_Specular_Cube.cso"));

			normalStep.AddBindable(CachedBuffer::GetBindable(gfx, bufferData, {{TargetPixelShader, 1}}));

			normalStep.AddBindable(Texture::GetBindable(gfx, diffuseTexture, 0, true));

			normalStep.AddBindable(Texture::GetBindable(gfx, normalTexture, 1, true));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pVertexShader.get()));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, false));

			normalStep.AddBindable(std::move(pVertexShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}

	{
		RenderTechnique outlineTechnique("outline");

		{
			RenderStep maskStep("outlineWriteMaskPass");

			outlineTechnique.AddRenderStep(maskStep);
		}

		{
			RenderStep maskStep("outlineMaskPass");


			std::shared_ptr pVertexShader = VertexShader::GetBindable(gfx, "VS.cso");

			DynamicConstantBuffer::BufferLayout PixelbufferLayout;
			PixelbufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

			DynamicConstantBuffer::BufferData pixelBufferData(std::move(PixelbufferLayout));
			*pixelBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = { 0.0f, 1.0f, 1.0f, 1.0f };

			maskStep.AddBindable(pVertexShader);

			maskStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			maskStep.AddBindable(CachedBuffer::GetBindable(gfx, pixelBufferData, {{TargetPixelShader, 1}}));

			maskStep.AddBindable(InputLayout::GetBindable(gfx, { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }, pVertexShader.get()));

			outlineTechnique.AddRenderStep(maskStep);
		}

		AddRenderTechnique(outlineTechnique);
	}
}

SimpleMesh Cube::GetNormalMesh(float scale)
{
	std::vector<DirectX::XMFLOAT3> vertices
	{
		{ -scale,-scale,-scale },
		{ scale, -scale, -scale },
		{ -scale,scale,-scale },
		{ scale,scale,-scale },
		{ -scale,-scale,scale },
		{ scale,-scale,scale },
		{ -scale,scale,scale },
		{ scale,scale,scale }
	};

	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);
	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));
	for (const auto& position : vertices)
		vertexBuffer.Emplace_Back(position);

	std::vector<UINT32> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};

	return { std::move(vertexBuffer), std::move(indices) };
}

SimpleMesh Cube::GetInsideDrawnMesh(float scale, bool withTextureCoords)
{
	/* vertices */

	const std::vector<DirectX::XMFLOAT3> vertexPositions = {

		//front
		{-scale, -scale, -scale},
		{-scale, scale, -scale},
		{scale, -scale, -scale},
		{scale, scale, -scale},

		//right
		{scale, -scale, -scale},
		{scale, scale, -scale},
		{scale, -scale, scale},
		{scale, scale, scale},

		//left
		{-scale, -scale, scale},
		{-scale, scale, scale},
		{-scale, -scale, -scale},
		{-scale, scale, -scale},

		//top
		{-scale, scale, -scale},
		{-scale, scale, scale},
		{scale, scale, -scale},
		{scale, scale, scale},

		//bottom
		{scale, -scale, -scale},
		{scale, -scale, scale},
		{-scale, -scale, -scale},
		{-scale, -scale, scale},

		//back
		{scale, -scale, scale},
		{scale, scale, scale},
		{-scale, -scale, scale},
		{-scale, scale, scale}
	};

	const std::vector<DirectX::XMFLOAT2> vertexTexturePositions = {
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);

	if (withTextureCoords)
		layout.Append(DynamicVertex::VertexLayout::Texture2D);

	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));

	for (size_t i = 0; i < vertexPositions.size(); i++)
		if (withTextureCoords)
			vertexBuffer.Emplace_Back(vertexPositions.at(i), vertexTexturePositions.at(i));
		else
			vertexBuffer.Emplace_Back(vertexPositions.at(i));

	/* indices */

	const std::vector<UINT32> cubeFaceIndices = { 1,0,2, 3,1,2 };
	std::vector<UINT32> indices = {};

	for (size_t i = 0; i < 6; i++)
		for (const auto& indice : cubeFaceIndices)
			indices.push_back(indice + i * 4);


	return { std::move(vertexBuffer), std::move(indices) };
}

SimpleMesh Cube::GetUnwrappedMesh(float scale, bool getExtendedStuff)
{
	/* vertices */

	const std::vector<DirectX::XMFLOAT3> vertexPositions = {

		//front
		{-scale, -scale, -scale},
		{-scale, scale, -scale},
		{scale, -scale, -scale},
		{scale, scale, -scale},

		//right
		{scale, -scale, -scale},
		{scale, scale, -scale},
		{scale, -scale, scale},
		{scale, scale, scale},

		//left
		{-scale, -scale, scale},
		{-scale, scale, scale},
		{-scale, -scale, -scale},
		{-scale, scale, -scale},

		//top
		{-scale, scale, -scale},
		{-scale, scale, scale},
		{scale, scale, -scale},
		{scale, scale, scale},

		//bottom
		{scale, -scale, -scale},
		{scale, -scale, scale},
		{-scale, -scale, -scale},
		{-scale, -scale, scale},

		//back
		{scale, -scale, scale},
		{scale, scale, scale},
		{-scale, -scale, scale},
		{-scale, scale, scale}
	};

	const std::vector<DirectX::XMFLOAT3> vertexNormals = {
		//front
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, -1.0f},

		//right
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},

		//left
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},

		//top
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},

		//bottom
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},

		//back
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f}
	};

	const std::vector<DirectX::XMFLOAT2> vertexTexturePositions = {
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	DynamicVertex::VertexLayout layout = DynamicVertex::VertexLayout{}.Append(DynamicVertex::VertexLayout::Position3D);

	if (getExtendedStuff)
	{
		layout.Append(DynamicVertex::VertexLayout::Normal);
		layout.Append(DynamicVertex::VertexLayout::Texture2D);
	}

	DynamicVertex::VertexBuffer vertexBuffer(std::move(layout));

	for (size_t i = 0; i < vertexPositions.size(); i++)
		if(getExtendedStuff)
			vertexBuffer.Emplace_Back(vertexPositions.at(i), vertexNormals.at(i), vertexTexturePositions.at(i));
		else
			vertexBuffer.Emplace_Back(vertexPositions.at(i));

	/* indices */

	const std::vector<UINT32> cubeFaceIndices = { 1,2,0, 3,2,1 };
	std::vector<UINT32> indices = {};

	for (size_t i = 0; i < 6; i++)
		for (const auto& indice : cubeFaceIndices)
			indices.push_back(indice + i * 4);


	return { std::move(vertexBuffer), std::move(indices) };
}