#include "SpherePBR.h"
#include "Sphere.h"
#include "BindableClassesMacro.h"

SpherePBR::SpherePBR(GFX& gfx, DirectX::XMFLOAT3 startingPosition)
	:
	SceneObject(startingPosition),
	m_roughness(0.56f),
	m_metalic(0.0f),
	m_color(DirectX::XMFLOAT3{ 0.0f, 1.0f, 1.0f }),
	m_emission(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }),
	m_reflectivity(DirectX::XMFLOAT3{ 0.286f, 0.286f, 0.286f })
{
	SetShape(this);

	SimpleMesh sphereModel = Sphere::GetMeshWithNormalsAndTextureMapping(1.0f, 30, 60);

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), sphereModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), sphereModel.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTransformConstBuffer = std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2);

	// pushing our const buffer to bindable list
	{
		DynamicConstantBuffer::BufferLayout layout;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
		floatInfo.v_min = 0.0f;
		floatInfo.v_max = 3.0f;

		layout.Add<DynamicConstantBuffer::DataType::Float>("mapMultipler", &floatInfo);


		DynamicConstantBuffer::BufferData bufferData(std::move(layout));
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("mapMultipler") = 1.0f;

		CachedBuffer::GetBindable(gfx, bufferData, 2, false, "mapMultiplerData");
	}

	{
		RenderTechnique shadowTechnique("shadow");

		{
			RenderStep shadowStep("shadowMappingPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Shadow_HeightMap.cso");

			shadowStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "mapMultiplerData"));

			shadowStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, false));

			shadowStep.AddBindable(Texture::GetBindable(gfx, "Images\\Textures\\patterned_wooden_wall_panel\\height.tga", 0, false, false));

			shadowStep.AddBindable(std::move(pVertexShader));

			shadowTechnique.AddRenderStep(shadowStep);
		}

		AddRenderTechnique(shadowTechnique);
	}

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_PBR_HeightMap.cso");

			{
				DynamicConstantBuffer::BufferLayout layout;

				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
				floatInfo.v_min = 0.0f;
				floatInfo.v_max = 1.0f;

				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiColorInfo colorInfo = {};
				colorInfo.flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSmallPreview;

				layout.Add<DynamicConstantBuffer::DataType::Float>("roughness", &floatInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float>("metalic", &floatInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float3>("diffuseColor", &colorInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float3>("emission", &colorInfo);
				layout.Add<DynamicConstantBuffer::DataType::Float3>("reflectivity", &colorInfo);


				DynamicConstantBuffer::BufferData bufferData(std::move(layout));
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("roughness") = m_roughness;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("metalic") = m_metalic;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("diffuseColor") = m_color;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("emission") = m_emission;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("reflectivity") = m_reflectivity;

				normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_PBR_NoMaps.cso"));

				normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 1, true));
			}

			normalStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "mapMultiplerData"));

			normalStep.AddBindable(BlendState::GetBindable(gfx, false));

			normalStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, false));

			normalStep.AddBindable(Texture::GetBindable(gfx, "Images\\Textures\\patterned_wooden_wall_panel\\height.tga", 0, false, false));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			normalStep.AddBindable(std::move(pVertexShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}
}