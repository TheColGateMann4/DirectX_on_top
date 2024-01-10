#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "DynamicConstantBuffer.h"
#include "Mesh.h"
#include "ShapeSceneObject.h"
#include <random>

class Cube : public ShapeSceneObject
{
public:
	Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture);

public:
	//void MakePropeties(GFX& gfx) override;

	virtual void RenderOnScene(RenderQueue& renderQueue) const noexcept(!IS_DEBUG) override
	{
		this->Render(renderQueue);
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