#include "Sheet.h"
#include "BindableClassesMacro.h"
#include "Application.h"
#include "Includes.h"
#include "SimpleMesh.h"
#include "TransformConstBufferWithPixelShader.h"

Sheet::Sheet(GFX& gfx, const UINT32 TesselationRatio, const UINT32 TextureRatio)
{
	struct Vertex
	{
		struct
		{
			FLOAT x, y, z;
		} position;
		struct
		{
			FLOAT u, v;
		} texture;
	};
	SimpleMesh TesselatedSheet = GetTesselatedMesh(TesselationRatio, TextureRatio);

	AddBindable(PixelShader::GetBindable(gfx, "PixelShaderTextureLightningwUV.cso"));

	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VertexShaderTextureLightningwUV.cso");
	ID3DBlob* pBlob = pVertexShader->GetByteCode();
	AddBindable(std::move(pVertexShader));

	std::string bufferName = std::to_string(TesselationRatio) + std::to_string(TextureRatio) + "SHEET";

	AddBindable(VertexBuffer::GetBindable(gfx, bufferName, TesselatedSheet.m_vertices));
	AddIndexBuffer(IndexBuffer::GetBindable(gfx, bufferName, TesselatedSheet.m_indices));

	AddBindable(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));

	AddBindable(Texture::GetBindable(gfx, "Images\\brickwall.jpg", 0));

	AddBindable(Texture::GetBindable(gfx, "Images\\brickwallUV.jpg", 1, true));

	AddBindable(PixelConstantBuffer<ModelMaterial>::GetBindable(gfx, m_constBuffer, 1));

	AddBindable(InputLayout::GetBindable(gfx, TesselatedSheet.GetLayout(), pBlob));

	AddBindable(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBindable(std::make_shared<TransformConstBufferWithPixelShader>(gfx, *this, 0, 2));
}

SimpleMesh Sheet::GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio)
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

	for (UINT32 row = 0; row < TesselationRatio + 1; row++)
		for (UINT32 column = 0; column < TesselationRatio + 1; column++)
		{
			vertices.Emplace_Back(
				DirectX::XMFLOAT3( column * lengthOfTriangle * 15, row * lengthOfTriangle * 15, 0 ),
				DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f),
				DirectX::XMFLOAT2((column * lengthOfTriangle) * textureRatio,(row * lengthOfTriangle) * textureRatio)
			);

			if (row < TesselationRatio && column < TesselationRatio)
			{
				//front
				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column);
				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				indices.push_back((row * (TesselationRatio + 1)) + column);

				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				indices.push_back((row * (TesselationRatio + 1)) + column + 1);
				indices.push_back((row * (TesselationRatio + 1)) + column);
				
				//back
				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column);
				indices.push_back((row * (TesselationRatio + 1)) + column);

				indices.push_back(((row + 1) * (TesselationRatio + 1)) + column + 1);
				indices.push_back((row * (TesselationRatio + 1)) + column);
				indices.push_back((row * (TesselationRatio + 1)) + column + 1);
			}
		}

	return { std::move(vertices), std::move(indices) };
}

DirectX::XMMATRIX Sheet::GetTranformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_rotation.z, m_rotation.y, m_rotation.x) *
		DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);// *
		//DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
}

void Sheet::SpawnControlWindow(GFX& gfx)
{
	if (ImGui::Begin("Brick Wall"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
		ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
		ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

		ImGui::Text("Rotation");
		ImGui::SliderAngle("rX", &m_rotation.x, -180.0, 180.0);
		ImGui::SliderAngle("rY", &m_rotation.y, -180.0, 180.0);
		ImGui::SliderAngle("rZ", &m_rotation.z, -180.0, 180.0);

		ImGui::Text("Scale");
		ImGui::SliderFloat("sX", &m_scale.x, 0.1f, 50.0f);
		ImGui::SliderFloat("sY", &m_scale.y, 0.1f, 50.0f);
		ImGui::SliderFloat("sZ", &m_scale.z, 0.1f, 50.0f);

		ImGui::Text("Propeties");
		bool changedP = ImGui::SliderFloat("specularIntensity", &m_constBuffer.specularIntensity, 0.1f, 100.0f, "%.1f");
		bool changedI = ImGui::SliderFloat("specularPower", &m_constBuffer.specularPower, 0.1f, 100.0f, "%.1f");
		bool changedM = ImGui::Checkbox("EnableNormalMap", &m_constBuffer.normalMapEnabled);

		bool changed = changedP || changedI || changedM;
		if (ImGui::Button("Reset"))
		{
			changed = true;
			this->Reset();
		}

		if (changed)
			UpdateConstBuffer(gfx);
	}
	ImGui::End();
}

void Sheet::Reset()
{
	m_constBuffer.specularIntensity = 0.8f;
	m_constBuffer.specularPower = 50.0f;
	m_constBuffer.normalMapEnabled = true;
	m_rotation = {};
	m_position = {};
	m_scale = {1.0f, 1.0f, 1.0f};
}

void Sheet::UpdateConstBuffer(GFX& gfx)
{
	PixelConstantBuffer<ModelMaterial>* pPixelConstBuffer = GetBindable<PixelConstantBuffer<ModelMaterial>>();
	pPixelConstBuffer->Update(gfx, m_constBuffer);
}