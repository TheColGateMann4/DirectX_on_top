#include "Node.h"
#include "Mesh.h"

void ModelNode::CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform) noexcept
{
	const auto finalTransform =
		(
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z) *	//added rotation
			DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z)				//added position
			)
		* DirectX::XMLoadFloat4x4(&m_baseTransform)												// original transform in node
		* parentTransform;																		// accumulated transform from upper nodes

	for (const auto& pMesh : m_pMeshes)
	{
		pMesh->SetTranformMatrix(finalTransform);
	}

	//passing accumulated transform to objects lower in hierarchy
	for (const auto& child : m_children)
	{
		child->CalculateSceneTranformMatrix(finalTransform);
	}
}

void ModelNode::RenderThisObjectOnScene() const noexcept(!IS_DEBUG)
{
	for (const auto& pMesh : m_pMeshes)
	{
		pMesh->Render();
	}
}

void ModelNode::LinkSceneObjectToPipeline(class RenderGraph& renderGraph)
{
	for (const auto& pMesh : m_pMeshes)
	{
		pMesh->LinkToPipeline(renderGraph);
	}
}

const char* ModelNode::GetName() const
{
	return m_nodeName.c_str();
}