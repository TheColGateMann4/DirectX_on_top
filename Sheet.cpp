#include "Sheet.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Includes.h"
#include "SimpleMesh.h"

Sheet::Sheet(GFX& gfx, DirectX::XMFLOAT3 startingPosition)
	:
	SceneObject(startingPosition)
{
	SetShape(this);

	SimpleMesh sheetModel = GetTesselatedMesh(1, 1, m_planeLength, 2.5f);

	std::string modelName = GetName() + std::to_string(m_planeLength);

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, modelName, sheetModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, modelName, sheetModel.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetDomainShader, 0} });

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_NoProjection.cso");

			{
				DynamicConstantBuffer::BufferLayout layout;
				layout.Add<DynamicConstantBuffer::DataType::Float>("deltaTime");

				DynamicConstantBuffer::BufferData bufferData(std::move(layout));
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("deltaTime") = 0.0f;

				deltaTimeCbuffer = std::make_shared<CachedBuffer>(gfx, bufferData, 1, TargetDomainShader);
				normalStep.AddBindable(deltaTimeCbuffer);
			}

			{
				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo stepnessInfo = {};
				stepnessInfo.v_min = 0.0f;
				stepnessInfo.v_max = 2.0f;
				stepnessInfo.format = "%.2f";

				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo speedInfo = {};
				speedInfo.v_min = 0.1f;
				speedInfo.v_max = 10.0f;
				speedInfo.format = "%.5f";


				DynamicConstantBuffer::BufferLayout layout;
				layout.Add<DynamicConstantBuffer::DataType::Float>("stepness", &stepnessInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float>("speed", &speedInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float>("scale", &speedInfo);

				DynamicConstantBuffer::BufferData bufferData(std::move(layout));
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("stepness") = 2.0f;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("speed") = 8.0f;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("scale") = 0.25f;


				normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 2, TargetDomainShader));
			}


			normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_Solid.cso"));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, sheetModel.GetLayout(), pVertexShader.get()));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, false));

			normalStep.AddBindable(std::move(pVertexShader));

			normalStep.AddBindable(HullShader::GetBindable(gfx, "HS.cso"));

			normalStep.AddBindable(DomainShader::GetBindable(gfx, "DS_Wave.cso"));

			{
				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo factorInfo = {};
				factorInfo.v_min = 0.1f;
				factorInfo.v_max = 20.0f;
				factorInfo.format = "%.2f";

				DynamicConstantBuffer::BufferLayout layout;
				layout.Add<DynamicConstantBuffer::DataType::Float>("tesselationFactor", &factorInfo);

				DynamicConstantBuffer::BufferData bufferData(std::move(layout));
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("tesselationFactor") = 10.0f;

				normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 0, TargetHullShader));
			}

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetHullShader));

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetDomainShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}

}

void Sheet::MakeAdditionalPropeties(GFX& gfx)
{
	if (ImGui::SliderFloat("length", &m_planeLength, 0.1f, 10.0f, "%.1f"))
	{
		SimpleMesh sheetModel = GetTesselatedMesh(1, 1, m_planeLength, 2.0f);

		std::string modelName = GetName() + std::to_string(m_planeLength);

		m_pIndexBuffer = IndexBuffer::GetBindable(gfx, modelName, sheetModel.m_indices);
		m_pVertexBuffer = VertexBuffer::GetBindable(gfx, modelName, sheetModel.m_vertices);
	}
}

SimpleMesh Sheet::GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio, float length, float scale)
{
	if (TesselationRatio == 0)
		std::abort();

	DynamicVertex::VertexLayout layout = 
		DynamicVertex::VertexLayout()
			.Append(DynamicVertex::VertexLayout::Position3D)
			.Append(DynamicVertex::VertexLayout::Normal)
			.Append(DynamicVertex::VertexLayout::Texture2D);

	DynamicVertex::VertexBuffer vertices(std::move(layout));
	
	std::vector<UINT32> indices;

	const float maxNormalizedPosition = 1.0f;
	const float minNormalizedPosition = 0.0f;
	const float lengthOfTriangle = (maxNormalizedPosition - minNormalizedPosition) / TesselationRatio;

	size_t repetitions = std::floor(length);
	float lastRowOffset = length - repetitions;

	float basePositionOffset = 0.0f;
	size_t baseIndex = 0;
	float farScale = scale;

	for (size_t i = 0; i <= repetitions; i++)
	{
		for (UINT32 row = 0; row < TesselationRatio + 1; row++)
			for (UINT32 column = 0; column < TesselationRatio + 1; column++)
			{
				if (i == repetitions)
					farScale *= lastRowOffset;

				vertices.Emplace_Back(
					DirectX::XMFLOAT3(column * lengthOfTriangle * scale, 0, row * lengthOfTriangle * scale),
					DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
					DirectX::XMFLOAT2((column * lengthOfTriangle) * textureRatio, (row * lengthOfTriangle) * textureRatio)
				);

				if (row < TesselationRatio && column < TesselationRatio)
				{
					//front
					indices.push_back(baseIndex + ((row + 1) * (TesselationRatio + 1)) + column);
					indices.push_back(baseIndex + ((row + 1) * (TesselationRatio + 1)) + column + 1);
					indices.push_back(baseIndex + (row * (TesselationRatio + 1)) + column);

					indices.push_back(baseIndex + ((row + 1) * (TesselationRatio + 1)) + column + 1);
					indices.push_back(baseIndex + (row * (TesselationRatio + 1)) + column + 1);
					indices.push_back(baseIndex + (row * (TesselationRatio + 1)) + column);
				}
			}

		basePositionOffset += scale;
		baseIndex += 6 * TesselationRatio * TesselationRatio;
	}

	return { std::move(vertices), std::move(indices) };
}

void Sheet::Update(GFX& gfx, float deltatime)
{
	DynamicConstantBuffer::BufferData bufferData = deltaTimeCbuffer->GetBufferData();

	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("deltaTime") += deltatime;

	deltaTimeCbuffer->Update(gfx, bufferData);
}