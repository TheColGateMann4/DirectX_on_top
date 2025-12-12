#pragma once
#include "Shape.h"
#include "SceneObject.h"

class SpherePBR : public SceneObject, public Shape
{
public:
	SpherePBR(GFX& gfx, DirectX::XMFLOAT3 startingPosition = { 0.0f, 0.0f, 0.0f });

public:
	void ChangeModel(GFX& gfx, const char* textureName);



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
		return "PBR_Sphere";
	}
	
	virtual void MakeAdditionalPropeties(GFX& gfx) override;

private:
	const std::vector<std::string> textureNameList = {"dark_acoustic_panel", "grey_porous_rock", "patterned_wooden_wall_panel", "space_station_foil", "violet_crystal", "quilted_black_leather", "golden_metal", "graffiti_tag_decals"};
	const char* selectedTextureName = textureNameList.at(textureNameList.size() - 2).c_str();
};