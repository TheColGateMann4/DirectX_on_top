#pragma once
#include "Shape.h"
#include "SceneObject.h"

class SpherePBR : public SceneObject, public Shape
{
public:
	SpherePBR(GFX& gfx, std::string texturePath, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

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
		return "PBR_Sphere";
	}

private:
	float m_roughness;
	float m_metalic;
	DirectX::XMFLOAT3 m_color;
	DirectX::XMFLOAT3 m_emission;
	DirectX::XMFLOAT3 m_reflectivity;
};