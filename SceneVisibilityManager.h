#pragma once
#include "Includes.h"
#include "ShaderUnorderedAccessView.h"

class GFX;
class Camera;
class Scene;

class SceneVisibilityManager
{
public:
	SceneVisibilityManager(GFX& gfx);

public:
	void ProcessVisibilityBuffer(GFX& gfx, Camera* camera, UINT highestIndexOnScene, std::vector<UINT8>* objectValidity);

	bool GetVisibility(size_t objectID) const;

	ShaderUnorderedAccessView* GetCubeBoundsUAV() const;

	ID3D11Buffer* GetMatrixBuffer() const;

private:
	std::shared_ptr<ShaderUnorderedAccessView> m_pModelBoundsUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pObjectsMatrixBuffer;
	std::vector<UINT8> m_visiblityData = {};
	bool initializedVector = false;

	static constexpr UINT32 startingBufferElementWidth = 10;
};