#include "SpherePBR.h"
#include "Sphere.h"
#include "BindableClassesMacro.h"
#include <Shlwapi.h>

SpherePBR::SpherePBR(GFX& gfx, DirectX::XMFLOAT3 startingPosition)
	:
	SceneObject(startingPosition)
{
	SetShape(this);

	SimpleMesh sphereModel = Sphere::GetMeshWithNormalsAndTextureMapping(1.0f, 30, 60);

	Sphere::CalculateTangents(sphereModel);

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), sphereModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), sphereModel.m_vertices);
	m_pTopology = nullptr;
	m_pTransformConstBuffer = TransformConstBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0}, {TargetPixelShader, 2}, {TargetDomainShader, 0} });


	DynamicConstantBuffer::BufferData bufferData;
	
	{
		DynamicConstantBuffer::BufferLayout layout;

		DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
		floatInfo.v_min = 0.0f;
		floatInfo.v_max = 3.0f;

		layout.Add<DynamicConstantBuffer::DataType::Float>("mapMultipler", &floatInfo);


		bufferData = std::move(layout);
		*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("mapMultipler") = 0.15f;
	}

	{
		RenderTechnique shadowTechnique("shadow");

		{
			RenderStep shadowStep("shadowMappingPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Shadow_Height.cso");

			shadowStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, TargetVertexShader));

			shadowStep.AddBindable(Texture::GetBindable(gfx, texturePath + "height.tga", 0, false, TargetVertexShader));

			shadowStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 2, TargetVertexShader));

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

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_PBR_Height.cso");

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

				normalStep.AddBindable(CachedBuffer::GetBindable(gfx, bufferData, {{TargetPixelShader, 1}}));
			}

			normalStep.AddBindable(std::make_shared<CachedBuffer>(gfx, bufferData, 1, TargetDomainShader));

			normalStep.AddBindable(BlendState::GetBindable(gfx, false));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, false));

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

			normalStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, TargetDomainShader));

			normalStep.AddBindable(HullShader::GetBindable(gfx, "HS_Test.cso"));

			normalStep.AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST));

			normalStep.AddBindable(DomainShader::GetBindable(gfx, "DS_Test.cso"));

			normalStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			normalStep.AddBindable(std::move(pVertexShader));

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetHullShader));

			normalStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetDomainShader));

			normalTechnique.AddRenderStep(normalStep);
		}

		AddRenderTechnique(normalTechnique);
	}

	ChangeModel(gfx, selectedTextureName);
}

void SpherePBR::ChangeModel(GFX& gfx, const char* textureName)
{
	std::string fullTextureName = "Images\\Textures\\";
	fullTextureName += textureName;
	fullTextureName += '\\';

	const char* heightMapTextureName = PathFileExistsA((fullTextureName + "height.tga").c_str()) ? "height.tga" : "height.jpg";

	{
		RenderStep* shadowStep = GetTechnique(0)->GetStep(0);

		shadowStep->RemoveBindable<Texture>("");

		shadowStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + heightMapTextureName, 0, false, TargetDomainShader));
	}

	{
		RenderStep* normalStep = GetTechnique(1)->GetStep(0);

		normalStep->RemoveBindable<Texture>("");

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + heightMapTextureName, 0, false, TargetDomainShader));

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + "diffuse.jpg", 0, false));

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + "normal.jpg", 1, false));

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + "roughness.jpg", 2, false));

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + "metallic.jpg", 3, false));

		normalStep->AddBindable(Texture::GetBindable(gfx, fullTextureName + "reflection.jpg", 4, false));
	}
}

void SpherePBR::MakeAdditionalPropeties(GFX& gfx)
{
	if (ImGui::BeginCombo("textureName", selectedTextureName))
	{
		for (int textureIndex = 0; textureIndex < textureNameList.size(); textureIndex++)
		{
			bool is_selected = (selectedTextureName == textureNameList.at(textureIndex));
			if (ImGui::Selectable(textureNameList.at(textureIndex).c_str(), is_selected))
			{
				selectedTextureName = textureNameList.at(textureIndex).c_str();
				ChangeModel(gfx, selectedTextureName);
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}