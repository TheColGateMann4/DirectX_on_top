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

	{
		RenderTechnique shadowTechnique("shadow");

		{
			RenderStep shadowStep("shadowMappingPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Shadow_Height.cso");

			shadowStep.AddBindable(InputLayout::GetBindable(gfx, sphereModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(SamplerState::GetBindable(gfx, SamplerState::CLAMP, 0, SamplerState::NEVER, SamplerState::BILINEAR, TargetDomainShader));

			{
				DynamicConstantBuffer::BufferLayout layout;

				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo floatInfo = {};
				floatInfo.v_min = 0.0f;
				floatInfo.v_max = 3.0f;

				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiIntInfo intInfo = {};
				intInfo.v_min = 1;
				intInfo.v_max = 4;

				layout.Add<DynamicConstantBuffer::DataType::Float>("mapMultipler", &floatInfo);
				layout.Add<DynamicConstantBuffer::DataType::Bool>("enableMultisampling");
				layout.Add<DynamicConstantBuffer::DataType::Int>("sampleNumber", &intInfo);


				DynamicConstantBuffer::BufferData heightMapFactor = std::move(layout);
				*heightMapFactor.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("mapMultipler") = 0.15f;
				*heightMapFactor.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("enableMultisampling") = TRUE;
				*heightMapFactor.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("sampleNumber") = 1;

				shadowStep.AddBindable(CachedBuffer::GetSharedBindable(gfx, heightMapFactor, { {TargetDomainShader, 1} }, "heightMapFactor"));
			}

			{
				DynamicConstantBuffer::ImguiAdditionalInfo::ImguiFloatInfo factorInfo = {};
				factorInfo.v_min = 1.0f;
				factorInfo.v_max = 50.0f;
				factorInfo.format = "%.2f";

				DynamicConstantBuffer::BufferLayout layout;
				layout.Add<DynamicConstantBuffer::DataType::Float>("tesselationFactor", &factorInfo);

				DynamicConstantBuffer::BufferData tesselationFactorData = std::move(layout);
				*tesselationFactorData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("tesselationFactor") = 10.0f;

				shadowStep.AddBindable(CachedBuffer::GetSharedBindable(gfx, tesselationFactorData, { {TargetHullShader, 0} }, "tesselationFactor"));
			}

			shadowStep.AddBindable(HullShader::GetBindable(gfx, "HS_Shadow_Test.cso"));

			shadowStep.AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST));

			shadowStep.AddBindable(DomainShader::GetBindable(gfx, "DS_Shadow_Test.cso"));

			shadowStep.AddBindable(std::move(pVertexShader));

			shadowStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetHullShader));

			shadowStep.AddPostRenderBindable(NullShader::GetBindable(gfx, TargetDomainShader));

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
				layout.Add<DynamicConstantBuffer::DataType::Float3>("insideColor", &colorInfo);

				DynamicConstantBuffer::BufferData bufferData(std::move(layout));
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("roughness") = 1.0f;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("metalic") = 1.0f;
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("diffuseColor") = { 1.0f, 1.0f, 1.0f };
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("emission") = { 0.0f, 0.0f, 0.0f };
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("reflectivity") = { 1.0f, 1.0f, 1.0f };
				*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("insideColor") = { 1.0f, 1.0f, 1.0f };

				normalStep.AddBindable(PixelShader::GetBindable(gfx, "PS_PBR_Diffuse_Normal_Roughness_Metallic.cso"));

				normalStep.AddBindable(CachedBuffer::GetBindable(gfx, bufferData, {{TargetPixelShader, 1}}));
			}

			normalStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "tesselationFactor"));

			normalStep.AddBindable(BlendState::GetBindable(gfx, false));

			normalStep.AddBindable(RasterizerState::GetBindable(gfx, false));

			normalStep.AddBindable(CachedBuffer::GetBindableWithoutCreation(gfx, "heightMapFactor"));

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