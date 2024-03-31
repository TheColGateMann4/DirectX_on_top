#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "DynamicConstantBuffer.h"
#include "SceneObject.h"
#include "Mesh.h"
#include <random>

class Cube : public SceneObject, public Shape
{
public:
	Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture);

public:
	virtual void LinkSceneObjectToPipeline(class RenderGraph& renderGraph) override
	{
		Shape::LinkToPipeline(renderGraph);
	}

	virtual void RenderOnScene() const noexcept(!IS_DEBUG) override
	{
		this->Render();
	}

	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override
	{
		return this->GetSceneTranformMatrix();
	}

public:
	std::string GetName() const override
	{
		return "Cube";
	}

private:
	static SimpleMesh GetNormalMesh(float scale);
	static SimpleMesh GetUnwrappedMesh(float scale, bool getExtendedStuff = false);

private:
	bool m_materialsDefined = false;
	bool m_objectGlowEnabled = true;
	bool m_objectMeshEnabled = true;

private:
	std::vector<std::shared_ptr<Bindable>> m_outlineBindables = {};
	const class IndexBuffer* m_pOutlineIndexBuffer = nullptr;
};