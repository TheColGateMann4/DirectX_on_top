#pragma once
#include "Includes.h"
#include "SceneObject.h"

class Mesh;

class ModelNode : public SceneObject
{
	friend class Model;

public:
	ModelNode(std::vector<Mesh*> pMeshes, const DirectX::XMMATRIX &transform, const char* nodeName) noexcept(!IS_DEBUG)
		: m_pMeshes(std::move(pMeshes)), m_nodeName(nodeName)
	{
		DirectX::XMStoreFloat4x4(&m_baseTransform, transform);
	}

public:
	virtual void CalculateSceneTranformMatrix(DirectX::XMMATRIX parentTransform = DirectX::XMMatrixIdentity()) noexcept override;

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override;

	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	virtual const char* GetName() const override;

private:
	std::vector<Mesh*> m_pMeshes;
	DirectX::XMFLOAT4X4 m_baseTransform;
	std::string m_nodeName;
};

