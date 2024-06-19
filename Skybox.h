#pragma once
#include "Shape.h"
#include "SceneObject.h"

class Skybox : public SceneObject, public Shape
{
public:
	Skybox(class GFX& gfx, float scale, std::string skyboxTexture);

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
		return "Skybox";
	}
};