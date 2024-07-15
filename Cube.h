#pragma once
#include "Shape.h"
#include "SimpleMesh.h"
#include "SceneObject.h"

class Cube : public SceneObject, public Shape
{
protected:
	Cube(class GFX& gfx, SimpleMesh&& mesh, std::string normalShaderName, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

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

protected:
	const char* GetName() const override
	{
		return "Cube";
	}

public:
	static SimpleMesh GetNormalMesh(float scale);
	static SimpleMesh GetInsideDrawnMesh(float scale, bool withTextureCoords = false, bool withNormals = false);
	static SimpleMesh GetUnwrappedMesh(float scale, bool withTextureCoords = false, bool withNormals = false);

private:
	bool m_materialsDefined = false;
	bool m_objectGlowEnabled = true;
	bool m_objectMeshEnabled = true;

private:
	std::vector<std::shared_ptr<Bindable>> m_outlineBindables = {};
	const class IndexBuffer* m_pOutlineIndexBuffer = nullptr;
};

class ColoredCube : public Cube
{
public:
	ColoredCube(class GFX& gfx, float scale, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });
};

class TexturedCube : public Cube
{
public:
	TexturedCube(class GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });
};