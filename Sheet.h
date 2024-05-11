#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "SceneObject.h"

class Sheet : public SceneObject, public Shape
{
public:
	Sheet(GFX& gfx, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

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

	const char* GetName() const override
	{
		return "Sheet";
	}

private:
	virtual void Update(GFX& gfx, float deltatime) override;

private:
	static SimpleMesh GetTesselatedMesh(const UINT32 TesselationRatio, const UINT32 textureRatio, float scale = 2.0f);

	std::shared_ptr<CachedBuffer> deltaTimeCbuffer;

	float m_planeLength = 2.0f;
};

