#pragma once
#include "Shape.h"

class CameraIndicator : public Shape
{
public:
	CameraIndicator(class GFX& gfx, Camera* parent);

public:
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

	void UpdateVertexBuffer(GFX& gfx);

	static std::shared_ptr<VertexBuffer> GetVertexBuffer(GFX& gfx, float aspectRatio, std::vector<D3D11_INPUT_ELEMENT_DESC>* layout = nullptr);

private:
	Camera* m_parent;
};

