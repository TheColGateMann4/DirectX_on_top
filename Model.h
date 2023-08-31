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
#include <random>
#include "ErrorMacros.h"

class Model
{
private:
	struct ModelMaterial {
		BOOL normalMapEnabled = TRUE;
		BOOL normalMapHasAlpha = FALSE;
		BOOL specularMapEnabled = TRUE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f,0.75f,0.75f };
		float specularMapWeight = 0.671f;

		float padding[4];
	};
	struct ModelMaterialNoMaps {
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;

		float padding[3];
	};

public:
	Model(GFX& gfx, std::string fileName);

private:
	static std::unique_ptr<Mesh> ParseMesh(GFX& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);

	std::unique_ptr<Node> ParseNode(const aiNode& node) noexcept;

public:
	void Draw(GFX& gfx) const noexcept(!IS_DEBUG);

public:
	void SpawnControlWindow();

private:
	std::unique_ptr<Node> m_pStartingNode;
	std::vector<std::unique_ptr<Mesh>> m_pMeshes;

private:
	Node* m_pressedNode = nullptr;
};