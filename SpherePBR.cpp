#include "SpherePBR.h"
#include "Sphere.h"
#include "BindableClassesMacro.h"

SpherePBR::SpherePBR(GFX& gfx, std::string texturePath, DirectX::XMFLOAT3 startingPosition)
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

	Sphere::CalculateTangents(sphereModel);

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), sphereModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), sphereModel.m_vertices);
	m_pTopology = nullptr;
	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0}, {TargetPixelShader, 2}, {TargetDomainShader, 0} });

	// pushing our const buffer to bindable list
	{
		DynamicConstantBuffer::BufferLayout layout;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
		floatInfo.v_min = 0.0f;
		floatInfo.v_max = 3.0f;

		layout.Add<DynamicConstantBuffer::DataType::Float>("mapMultipler", &floatInfo);


		DynamicConstantBuffer::BufferData bufferData(std::move(layout));
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("mapMultipler") = 0.15f;

		CachedBuffer::GetBindable(gfx, bufferData, 2, TargetVertexShader, "mapMultiplerData");
	}

	{
		RenderTechnique shadowTechnique("shadow");

		{
			RenderStep shadowStep("shadowMappingPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Shadow_Height.cso");

			shadowStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "mapMultiplerData"));

			shadowStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, false));

			shadowStep.AddBindable(Texture::GetBindable(gfx, texturePath + "height.tga", 0, false, TargetVertexShader));

			shadowStep.AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			shadowStep.AddBindable(std::move(pVertexShader));

			shadowTechnique.AddRenderStep(shadowStep);
		}

		AddRenderTechnique(shadowTechnique);
	}

	{
		RenderTechnique normalTechnique("normal");

		{
			RenderStep normalStep("normalPass");

			//std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_PBR_Height.cso");
			
			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_PBR.cso");

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

				normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_PBR.cso"));

				//normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_PBR_Diffuse_Normal_Roughness_Metallic.cso"));

				normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 1, TargetPixelShader));
			}

			normalStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "mapMultiplerData"));

			normalStep.AddBindable(BlendState::GetBindable(gfx, false));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, true));

			normalStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, false));

// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "height.tga", 0, false, TargetVertexShader));
// 
// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "diffuse.jpg", 0, false));
// 
// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "normal.jpg", 1, false));
// 
// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "roughness.jpg", 2, false));
// 
// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "metallic.jpg", 3, false));
// 
// 			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "reflection.jpg", 4, false));

			normalStep.AddBindable(Texture::GetBindable(gfx, texturePath + "height.tga", 0, false, TargetHullShader));

			normalStep.AddBindable(HullShader::GetBindable(gfx, "HS_test.cso"));

			normalStep.AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST));

			normalStep.AddBindable(DomainShader::GetBindable(gfx, "DS_test.cso"));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			normalStep.AddBindable(std::move(pVertexShader));

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetHullShader));

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetDomainShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}
}