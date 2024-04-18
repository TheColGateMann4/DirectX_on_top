#pragma once
#include "Shape.h"
#include "SimpleMesh.h"
#include "SceneObject.h"
#include <random>

class Cube : public SceneObject, public Shape
{
public:
	Cube(class GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override
	{
		Shape::LinkToPipeline(renderGraph);
	}

	virtual void RenderThisObjectOnScene() const noexcept(!IS_DEBUG) override
	{
		this->Render();
	}

	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return this->GetSceneTranformMatrix();
	}

public:
	const char* GetName() const override
	{
		return "Cube";
	}

private:
	static SimpleMesh GetNormalMesh(float scale);
	static SimpleMesh GetInsideDrawnMesh(float scale, bool withTextureCoords = false);
	static SimpleMesh GetUnwrappedMesh(float scale, bool getExtendedStuff = false);

private:
	bool m_materialsDefined = false;
	bool m_objectGlowEnabled = true;
	bool m_objectMeshEnabled = true;

private:
	std::vector<std::shared_ptr<Bindable>> m_outlineBindables = {};
	const class IndexBuffer* m_pOutlineIndexBuffer = nullptr;
};