#include "Cube.h"
#include "BindableClassesMacro.h"
#include "Vertex.h"
#include "imgui/imgui.h"

Cube::Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, bool enableOutline)
	: m_glowEnabled(enableOutline)
{
	SimpleMesh CubeModel = GetUnwrappedMesh(scale, true);

	std::vector<std::shared_ptr<Bindable>> meshBindables = {};

	meshBindables.push_back(VertexBuffer::GetBindable(gfx, "Cube", CubeModel.m_vertices));

	std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Phong_Cube.cso");
	ID3DBlob* pBlob = pVertexShader->GetByteCode();
	meshBindables.push_back(std::move(pVertexShader));

	meshBindables.push_back(PixelShader::GetBindable(gfx, "PS_Phong_Texture_Normals_Specular_Cube.cso"));

	DynamicConstantBuffer::BufferLayout layout;
	layout.Add<DynamicConstantBuffer::DataType::Float>("specularIntensity");
	layout.Add<DynamicConstantBuffer::DataType::Float>("specularPower");
	layout.Add<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled");

	DynamicConstantBuffer::BufferData bufferData(std::move(layout));
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity") = 2.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower") = 150.0f;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled") = TRUE;

	meshBindables.push_back(std::make_shared<CachedBuffer>(gfx, bufferData, 1, true));

	meshBindables.push_back(IndexBuffer::GetBindable(gfx, "Cube", CubeModel.m_indices));

	meshBindables.push_back(Texture::GetBindable(gfx, diffuseTexture, 0));

	meshBindables.push_back(Texture::GetBindable(gfx, normalTexture, 1));

	meshBindables.push_back(InputLayout::GetBindable(gfx, CubeModel.GetLayout(), pBlob));

	meshBindables.push_back(Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	meshBindables.push_back(SamplerState::GetBindable(gfx, D3D11_TEXTURE_ADDRESS_WRAP));

	meshBindables.push_back(DepthStencil::GetBindable(gfx, DepthStencil::Write));

	m_mesh = std::make_unique<Mesh>(gfx, std::move(meshBindables));

	if (enableOutline)
		m_outlineBindables = getOutline(gfx);
}

std::vector<std::shared_ptr<Bindable>> Cube::getOutline(GFX& gfx)
{
	std::vector<std::shared_ptr<Bindable>> outlineBindables = m_mesh->getAllBindables(m_pOutlineIndexBuffer);

	for (size_t i = 0; i < outlineBindables.size(); i++)
	{
		if (const PixelShader* pBindable = dynamic_cast<PixelShader*>(outlineBindables[i].get()))
		{
			outlineBindables.erase(outlineBindables.begin() + i);
		}
		else if (const VertexShader* pBindable = dynamic_cast<VertexShader*>(outlineBindables[i].get()))
		{
			outlineBindables.erase(outlineBindables.begin() + i);
		}
		else if (const InputLayout* pBindable = dynamic_cast<InputLayout*>(outlineBindables[i].get()))
		{
			outlineBindables.erase(outlineBindables.begin() + i);
		}
		else if (const CachedBuffer* pBindable = dynamic_cast<CachedBuffer*>(outlineBindables[i].get()))
		{
			outlineBindables.erase(outlineBindables.begin() + i);
		}
		else if (const DepthStencil* pBindable = dynamic_cast<DepthStencil*>(outlineBindables[i].get()))
		{
			outlineBindables.erase(outlineBindables.begin() + i);
		}
	}


	std::shared_ptr pVertexShader = VertexShader::GetBindable(gfx, "VS_Outline.cso");
	ID3DBlob* pBlob = pVertexShader->GetByteCode();


	DynamicConstantBuffer::BufferLayout bufferLayout;
	bufferLayout.Add<DynamicConstantBuffer::DataType::Float4>("color");

	DynamicConstantBuffer::BufferData bufferData(std::move(bufferLayout));
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float4>("color") = { 0.0f, 1.0f, 1.0f, 1.0f };


	outlineBindables.push_back(pVertexShader);
	outlineBindables.push_back(PixelShader::GetBindable(gfx, "PS_Solid.cso"));
	outlineBindables.push_back(std::make_shared<CachedBuffer>(gfx, bufferData, 1, true));
	outlineBindables.push_back(InputLayout::GetBindable(gfx, { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } }, pBlob));
	outlineBindables.push_back(DepthStencil::GetBindable(gfx, DepthStencil::Mask));

	return outlineBindables;
}

void Cube::Draw(GFX& gfx) const noexcept(!IS_DEBUG)
{
	const auto finalTransform =
		(
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
			DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z)
			);

	m_mesh->Draw(gfx, finalTransform);
}

void Cube::DrawWithOutline(GFX& gfx) const noexcept(!IS_DEBUG)
{
	if (m_glowEnabled)
	{
		for (auto& b : m_outlineBindables)
		{
			b->Bind(gfx);
		}
		gfx.DrawIndexed(m_pOutlineIndexBuffer->GetCount());
	}

	Draw(gfx);
}

void Cube::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
	m_scale = { 1.0f, 1.0f, 1.0f };
}

void Cube::MakePropeties(GFX & gfx, float deltaTime)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Position");
	ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
	ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
	ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

	ImGui::Text("Rotation");
	ImGui::SliderFloat("rX", &m_rotation.x, -std::_Pi, std::_Pi);
	ImGui::SliderFloat("rY", &m_rotation.y, -std::_Pi, std::_Pi);
	ImGui::SliderFloat("rZ", &m_rotation.z, -std::_Pi, std::_Pi);

	ImGui::Text("Scale");
	ImGui::SliderFloat("sX", &m_scale.x, 0.01f, 100.0f);
	ImGui::SliderFloat("sY", &m_scale.y, 0.01f, 100.0f);
	ImGui::SliderFloat("sZ", &m_scale.z, 0.01f, 100.0f);

	if (ImGui::Button("Reset"))
		ResetLocalTransform();

	CachedBuffer* cachedBuffer = m_mesh->GetBindable<CachedBuffer>();

	if (!materialsDefined)
		shaderMaterial = cachedBuffer->constBufferData;


	bool powerChanged = false, intensityChanged = false, normalMapStateChanged = false;

	powerChanged = ImGui::SliderFloat("specularPower", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularPower"), 0.001f, 150.0f);
	intensityChanged = ImGui::SliderFloat("specularIntensity", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Float>("specularIntensity"), 0.001f, 150.0f);
	normalMapStateChanged = ImGui::Checkbox("normalMapEnabled", shaderMaterial.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("normalMapEnabled"));

	if (powerChanged || intensityChanged || normalMapStateChanged)
		cachedBuffer->Update(gfx, shaderMaterial);

	materialsDefined = true;

	ImGui::Text("Glow Settings");

	if (ImGui::Checkbox("enable", &m_glowEnabled))
		m_outlineBindables = getOutline(gfx);

	if (m_glowEnabled)
	{
		for (const auto& outlineBindable : m_outlineBindables)
		{
			if (CachedBuffer* cachedOutlineBuffer = dynamic_cast<CachedBuffer*>(outlineBindable.get()))
			{
				DynamicConstantBuffer::BufferData bufferData = cachedOutlineBuffer->constBufferData;

				bool outlineColorChanged = ImGui::ColorPicker3("color", (float*)bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Float3>("color"));

				if (outlineColorChanged)
					cachedOutlineBuffer->Update(gfx, bufferData);
			}
		}
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

SimpleMesh Cube::GetUnwrappedMesh(float scale, bool getExtendedStuff)
{
	/* vertices */

	const std::vector<DirectX::XMFLOAT3> vertexPositions = {

		//front
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},

		//right
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},

		//left
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},

		//top
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},

		//bottom
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},

		//back
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f}
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

DirectX::XMMATRIX Cube::GetTranformMatrix() const noexcept
{
	return m_mesh->GetTranformMatrix();
}