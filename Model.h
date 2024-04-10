#pragma once
#include "Includes.h"
#include "SceneObject.h"
#include "Node.h"

class Model : public SceneObject
{
public:
	Model(GFX& gfx, std::string fileName, float scale = 1.0f);

private:
	static std::unique_ptr<Mesh> ParseMesh(GFX& gfx, const struct aiMesh& mesh, const struct aiMaterial* const* pMaterials, std::string &modelPath, std::string &fileName, float scale);

	std::unique_ptr<ModelNode> ParseNode(const struct aiNode& node) noexcept;

public:

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override
	{

	}

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override;

	void MakeHierarchy(GFX& gfx) override;

public:
	const char* GetName() const override
	{
		return m_fileName.c_str();
	}

private:
	std::vector<std::unique_ptr<Mesh>> m_pMeshes;

private:
	float m_scale = 1.0f;
	std::string m_filePath = "";
	std::string m_fileName = "";
};