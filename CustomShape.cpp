#include "CustomShape.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Vertex.h"
#include "Includes.h"
#include "imgui/imgui.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

CustomShape::CustomShape(GFX& gfx, std::string objpath, DirectX::XMFLOAT3 color, UINT32 objectNumber)
{
	this->m_objectNumber = objectNumber;
	this->m_defaultColor = color;
	if (!IsStaticInitialized())
	{
		DynamicVertex::VertexBuffer vertexBuffer(std::move(
			DynamicVertex::VertexLayout()
				.Append(DynamicVertex::VertexLayout::Position3D)
				.Append(DynamicVertex::VertexLayout::Normal)
		));
		std::vector<UINT32> indices;

		{//model stuff
			Assimp::Importer importer;
			auto model = importer.ReadFile(objpath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
			auto mesh = model->mMeshes[0];

			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				vertexBuffer.Emplace_Back<>(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i])
				);
			}

			indices.reserve(mesh->mNumFaces * 3);
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				const auto& face = mesh->mFaces[i];
				assert(face.mNumIndices == 3);
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
		}
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertexBuffer));

		std::unique_ptr<VertexShader> pVertexShader = std::make_unique<VertexShader>(gfx, L"VertexPhongLightningShader.cso");
		ID3DBlob* pBlob = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));


		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PixelPhongLightningShader.cso"));

		AddStaticIndexBufferBind(std::make_unique<IndexBuffer>(gfx, indices));

		AddStaticBind(std::make_unique<InputLayout>(gfx, vertexBuffer.GetLayout().GetDirectXLayout(), pBlob));

		AddStaticBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		GetIndexBufferFromVector();
	}

	this->Reset();
	AddBindable(std::make_unique<PixelConstantBuffer<ModelMaterial>>(gfx, m_material, 1));

	AddBindable(std::make_unique<TransformConstBuffer>(gfx, *this));

	DirectX::XMStoreFloat3x3(
		&m_scale,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
	);
}

VOID CustomShape::Update(FLOAT DeltaTime) noexcept
{

}

DirectX::XMMATRIX CustomShape::GetTranformMatrix() const noexcept
{
	return
		DirectX::XMLoadFloat3x3(&m_scale) *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
		DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
}

VOID CustomShape::SpawnControlWindow(GFX& gfx) noexcept
{
	std::string windowName = "Object " + std::to_string(m_objectNumber);
	bool changed = false;
	if (ImGui::Begin(windowName.c_str()))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
		ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
		ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

		ImGui::Text("Rotation");
		ImGui::SliderFloat("rX", &m_rotation.x, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rY", &m_rotation.y, -std::_Pi, std::_Pi);
		ImGui::SliderFloat("rZ", &m_rotation.z, -std::_Pi, std::_Pi);

		ImGui::Text("Scale");
		ImGui::SliderFloat("sX", &m_scale._11, 0.1f, 30.0f);
		ImGui::SliderFloat("sY", &m_scale._22, 0.1f, 30.0f);
		ImGui::SliderFloat("sZ", &m_scale._33, 0.1f, 30.0f);

		ImGui::Text("Color");
		const bool changedC =  ImGui::ColorEdit3("Material", &m_material.color.x, ImGuiColorEditFlags_NoAlpha);

		ImGui::Text("Shinyness");
		const bool changedSI = ImGui::SliderFloat("Specular Intensity", &m_material.specularIntensity, 0.00001f, 1.0f);
		const bool changedSP = ImGui::SliderFloat("Specular Power", &m_material.specularPower, 0.00001f, 50.0f);
		changed = changedC || changedSI || changedSP;

		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();

	if (changed)
		SyncConstBuffer(gfx);
}

VOID CustomShape::Reset() noexcept
{
	m_position = { float(3 * m_objectNumber), 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };

	DirectX::XMStoreFloat3x3(
		&m_scale,
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f)
	);

	m_material.color = m_defaultColor;
	m_material.specularIntensity = 0.6f;
	m_material.specularPower = 30.0f;
}

VOID CustomShape::SyncConstBuffer(GFX& gfx) noexcept(!IS_DEBUG)
{
	auto pPixelShaderCbuff = GetBindable<PixelConstantBuffer<ModelMaterial>>();
	assert(pPixelShaderCbuff != nullptr);
	pPixelShaderCbuff->Update(gfx, this->m_material); 
}