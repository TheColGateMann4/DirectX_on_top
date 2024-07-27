#include "SceneObject.h"
#include "imgui/imgui.h"
#include "Shape.h"

#include "ShaderUnorderedAccessView.h"
#include "ComputeShader.h"
#include "Camera.h"
#include "VertexBuffer.h"

SceneObject::SceneObject(DirectX::XMFLOAT3 startingPosition)
{
	m_position = startingPosition;
}

void SceneObject::LinkChildrenToPipeline(RenderGraph& renderGraph)
{
	for (auto& child : m_children)
	{
		child->LinkSceneObjectToPipeline(renderGraph);
		child->LinkChildrenToPipeline(renderGraph);
	}
}

void SceneObject::OnHierarchyFocus()
{

}

void SceneObject::OnHierarchyUnfocus()
{

}

void SceneObject::Update(GFX& gfx, float deltatime)
{

}

void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
{
	child->m_parent = this;

	m_children.push_back(std::move(child));
}

void SceneObject::RenderOnScene() const noexcept(!IS_DEBUG)
{
	this->RenderThisObjectOnScene();

	this->RenderChildrenOnScene();
}

void SceneObject::RenderChildrenOnScene() const noexcept(!IS_DEBUG)
{
	for (const auto& child : m_children)
		child->RenderOnScene();
}

void SceneObject::MakeHierarchy(GFX& gfx)
{
	ImGui::Columns(2, nullptr, true);

	SceneObject* previous = m_pressedNode;

	this->GenerateTree(previous);

	m_pressedNode = previous;
}

void SceneObject::GenerateTree(SceneObject*& pressedNode)
{
	if (!m_visibleInHierarchy)
		return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

	bool haveVisibleChildren = HaveVisibleChildren();

	if (!haveVisibleChildren)
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (m_pressed)
		flags |= ImGuiTreeNodeFlags_Selected;

	const bool nodeExpanded = ImGui::TreeNodeEx(GetOriginalName(true).c_str(), flags);

	if (ImGui::IsItemClicked())
		if (pressedNode != this)
		{
			if (pressedNode != nullptr)
				pressedNode->SetPressedState(false);

			m_pressed = true;
			pressedNode = this;
		}
		else
		{
			m_pressed = false;
			pressedNode = nullptr;
		}

	if (nodeExpanded)
	{
		if(haveVisibleChildren)
			for (const auto& child : m_children)
				child->GenerateTree(pressedNode);

		ImGui::TreePop();
	}
}

void SceneObject::MakeTransformPropeties(GFX& gfx)
{
	if (!GetPressedState())
		return;

	ImGui::Text("Position");
	ImGui::SliderFloat("pX", &m_position.x, -30.0f, 30.0f);
	ImGui::SliderFloat("pY", &m_position.y, -30.0f, 30.0f);
	ImGui::SliderFloat("pZ", &m_position.z, -30.0f, 30.0f);

	ImGui::Text("Rotation");
	ImGui::SliderAngle("rX", &m_rotation.x, -180.0f, 180.0f);
	ImGui::SliderAngle("rY", &m_rotation.y, -180.0f, 180.0f);
	ImGui::SliderAngle("rZ", &m_rotation.z, -180.0f, 180.0f);

	if (ImGui::Button("Reset"))
		ResetLocalTransform();
}

void SceneObject::MakePropeties(GFX& gfx)
{
	if (m_shape == nullptr || !GetPressedState())
		return;

	for (auto& technique : m_shape->m_techniques)
	{
		// Setting category text so we know what are we working on in inspector
		{
			bool techniqueActive = technique.GetTechniqueActive();

			ImU32 textColor = techniqueActive ? IM_COL32(0, 255, 0, 255) : IM_COL32(128, 128, 128, 255);
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);

			if (ImGui::Checkbox(technique.GetName().c_str(), &techniqueActive))
				technique.SetTechniqueActive(techniqueActive);

			ImGui::PopStyleColor();
		}


		// Writing off constant buffers
		{
			std::vector< std::pair<std::string, std::vector<CachedBuffer*>> > allStepBuffers = {};
			technique.GetEveryBindableOfType<CachedBuffer>(allStepBuffers);

			for (auto& stepBufferData : allStepBuffers)
			{
				// Writing the name of step se we know what we are working on - again
				// 		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
				//		ImGui::Text(stepBufferData.first.c_str());
				//		ImGui::PopStyleColor();

				// Writing off stuff from each buffer to set by user
				for (auto& constantBuffer : stepBufferData.second)
				{
					DynamicConstantBuffer::BufferData bufferData = constantBuffer->GetBufferData();
					if (bufferData.MakeImguiMenu())
						constantBuffer->Update(gfx, bufferData);
				}
			}
		}
	}
}

void SceneObject::MakeAdditionalPropeties(GFX& gfx)
{

}

void SceneObject::SetVisibilityInHierarchy(bool visibility)
{
	m_visibleInHierarchy = visibility;
}

bool SceneObject::GetVisibilityInHierarchy() const
{
	return m_visibleInHierarchy;
}

bool SceneObject::HaveVisibleChildren() const
{
	if (m_children.empty())
		return false;

	for (const auto& child : m_children)
		if (child->GetVisibilityInHierarchy())
			return true;

	return false;
}

void SceneObject::ResetLocalTransform() noexcept
{
	m_position = {};
	m_rotation = {};
}

DirectX::XMMATRIX SceneObject::GetSceneTranformMatrix() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

void SceneObject::CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform) noexcept
{
	const auto finalTransform =
		(
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z) *
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z)
			)
		* parentTransform;

	for (const auto& pChild : m_children)
	{
		pChild->CalculateSceneTranformMatrix(finalTransform);
	}

	DirectX::XMStoreFloat4x4(&m_transform , finalTransform);
}

DirectX::XMFLOAT3 SceneObject::GetWorldPosition() const
{
	DirectX::XMFLOAT3 result = m_position;
	SceneObject* parent = m_parent;

	// using nasa loop conventions hehe, gotta keep in mind that 128 is parent->child connections
	for(int i = 0; i < 128; i++)
	{
		if (parent == nullptr)
			return result;

		const DirectX::XMVECTOR vecChildPos = XMLoadFloat3(&result);
		const DirectX::XMVECTOR vecResult = XMVector3Transform(vecChildPos, parent->GetSceneTranformMatrix());
		DirectX::XMStoreFloat3(&result, vecResult);

		parent = parent->m_parent;
	}

	return result;
}

DirectX::XMFLOAT3 SceneObject::GetWorldRotation() const
{
	DirectX::XMFLOAT3 result = m_rotation;
	SceneObject* parent = m_parent;

	for (int i = 0; i < 128; i++)
	{
		if (parent == nullptr)
			return result;

		result.x += parent->m_rotation.x;
		result.y += parent->m_rotation.y;
		result.z += parent->m_rotation.z;

		parent = parent->m_parent;
	}

	return result;
}

const char* SceneObject::GetNameSpecialStatus() const
{
	return "";
}

std::string SceneObject::GetOriginalName(bool withStatus) const
{
	std::string result = GetName();

	if (m_sceneNameIndex > 0)
		result += "_" + std::to_string(m_sceneNameIndex);

	if(withStatus)
		result.append(GetNameSpecialStatus());

	return result;
}

void SceneObject::SetVisibility(std::vector<UINT8>& visibilityData)
{
	if (m_shape != nullptr)
		m_shape->m_visible = visibilityData.at(m_sceneIndex);

	for (const auto& pChild : m_children)
		pChild->SetVisibility(visibilityData);
}

void SceneObject::PushObjectMatrixToBuffer(std::vector<DirectX::XMMATRIX>& matrixData) const
{
	if(m_shape != nullptr)
		matrixData.at(m_sceneIndex) = DirectX::XMMatrixTranspose(GetSceneTranformMatrix());

	for (const auto& pChild : m_children)
		pChild->PushObjectMatrixToBuffer(matrixData);
}

void SceneObject::InitialzeSceneObject(INT32 sceneIndex, size_t repeatingNameIndex, std::vector<UINT8>& validityData, GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer)
{
	m_InitialzeSceneObject(sceneIndex, repeatingNameIndex, validityData, gfx, pModelCubeRWBuffer);
}

void SceneObject::m_InitialzeSceneObject(INT32& internalSceneIndex, size_t repeatingNameIndex, std::vector<UINT8>& validityData, GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer)
{
	bool isValidVisibleObject = m_shape != nullptr;

	internalSceneIndex++;

	validityData.at(internalSceneIndex) = static_cast<UINT8>(isValidVisibleObject);

	SetSceneIndexes(internalSceneIndex, repeatingNameIndex);

	if (isValidVisibleObject)
		GenerateBoundCube(gfx, pModelCubeRWBuffer);

	for (const auto& pChild : m_children)
		pChild->m_InitialzeSceneObject(internalSceneIndex, repeatingNameIndex, validityData, gfx, pModelCubeRWBuffer);
}

UINT32 SceneObject::GetNumChildren(bool getChildrenOfChildren) const
{
	return m_GetNumberOfChildren(true, getChildrenOfChildren);
}

UINT32 SceneObject::m_GetNumberOfChildren(bool firstOneCalled, bool getChildrenOfChildren) const
{
	UINT32 result = firstOneCalled ? 0 : 1;

	for (UINT32 i = 0; i < m_children.size(); i++)
		result += m_children.at(i)->m_GetNumberOfChildren(false, getChildrenOfChildren);

	return result;
}

void SceneObject::SetSceneIndexes(size_t sceneIndex, size_t repeatingNameIndex)
{
	m_sceneIndex = sceneIndex;
	m_sceneNameIndex = repeatingNameIndex;
}

void SceneObject::GenerateBoundCube(GFX& gfx, ShaderUnorderedAccessView* pModelCubeRWBuffer)
{
	HRESULT hr;

	// binding cube UAV
	{
		pModelCubeRWBuffer->Bind(gfx);
	}

	// giving vertex buffer as input to our shader
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pConstBufferView;

		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
			shaderResourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
			shaderResourceViewDesc.Buffer.FirstElement = 0;
			shaderResourceViewDesc.Buffer.NumElements = m_shape->m_pVertexBuffer->GetBufferByteSize() / 4;

			THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateShaderResourceView(m_shape->m_pVertexBuffer->Get(), &shaderResourceViewDesc, &pConstBufferView));
		}

		THROW_INFO_EXCEPTION(GFX::GetDeviceContext(gfx)->CSSetShaderResources(0, 1, pConstBufferView.GetAddressOf()));
	}

	// binding info about our vertex structure
	{
		//			cbuffer verticesInfo : register(b0)
		//			{
		//				uint positionOffsetInStructureInFloatIndexes;
		//				uint structureSizeInFloatIndexes;
		//				uint targetResultID;
		//			};

		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

		{
			const DynamicVertex::VertexLayout& vertexLayout = m_shape->m_pVertexBuffer->GetLayout();

			UINT pData[4] =
			{
				// in shaders we operate on float arrays, and every thread doesn't need to do the same operation, that's why we divide our values by 4 here
				vertexLayout.GetByIndex(vertexLayout.GetIndexOfElement(DynamicVertex::VertexLayout::VertexComponent::Position3D)).GetOffset() / 4,
				vertexLayout.GetByteSize() / 4,
				m_sceneIndex,  // target index of our model
				0	//padding
			};

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = sizeof(pData);
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = NULL;
			bufferDesc.MiscFlags = NULL;
			bufferDesc.StructureByteStride = sizeof(UINT);

			D3D11_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pSysMem = pData;

			THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&bufferDesc, &subResourceData, &pBuffer));
		}

		GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(0, 1, pBuffer.GetAddressOf());
	}

	//compute shader for making cube
	{
		ComputeShader::GetBindable(gfx, "CS_BoundsCubeOfModel.cso")->Bind(gfx);
	}

	// running compute shader for generating cube that contains model fully
	{
		gfx.Dispatch({ 1,1,1 });
	}
}