#pragma once
#include "Includes.h"
#include "Node.h"
#include "Bindable.h"
#include "BindableClassesMacro.h"
#include "BindableList.h"
#include "Vertex.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "imgui/imgui.h"
#include "ErrorMacros.h"
#include "SceneObject.h"

class Model : public SceneObject
{
public:
	Model(GFX& gfx, std::string fileName, float scale = 1.0f);

private:
	static std::unique_ptr<Mesh> ParseMesh(GFX& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, std::string &modelPath, std::string &fileName, float scale);

	std::unique_ptr<Node> ParseNode(const aiNode& node) noexcept;

public:

	virtual void RenderOnScene() const noexcept(!IS_DEBUG) override
	{
		m_pStartingNode->Render(DirectX::XMMatrixIdentity());
	}

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	void MakeHierarchy(GFX& gfx) override;
	virtual void MakeTransformPropeties(GFX& gfx) override;

public:
	const char* GetName() const override
	{
		return m_fileName.c_str();
	}

private:
	std::unique_ptr<Node> m_pStartingNode;
	std::vector<std::unique_ptr<Mesh>> m_pMeshes;

private:
	float m_scale = 1.0f;
	std::string m_filePath = "";
	std::string m_fileName = "";

public:
	Node* m_pressedNode = nullptr;
};