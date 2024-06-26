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
	void ProcessVisibilityBuffer(GFX& gfx, Camera* camera, INT32 highestIndexOnScene, std::vector<UINT8>* objectValidity);

	void UpdateTransformBuffer(GFX& gfx);

	std::vector<DirectX::XMMATRIX>& GetMatrixBuffer();

	void ResizeBuffers(GFX& gfx, INT32 newHighestObjectID);

public:
	std::vector<UINT8>& GetVisibilityBuffer();

	ShaderUnorderedAccessView* GetCubeBoundsUAV() const;

private:
	std::vector<DirectX::XMMATRIX> m_modelsMatrixData;
	std::shared_ptr<ShaderUnorderedAccessView> m_pModelBoundsUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pObjectsMatrixBuffer;
	std::vector<UINT8> m_visiblityData = {};
	bool initializedVector = false;

	static constexpr UINT32 startingBufferElementWidth = 10;
	UINT32 currentElementWidth = startingBufferElementWidth;
};