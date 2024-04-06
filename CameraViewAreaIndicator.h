#pragma once
#include "Shape.h"

class GFX;

class CameraViewAreaIndicator : public Shape
{
public:
	CameraViewAreaIndicator(GFX& gfx, Camera* parent);

public:
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

	static std::shared_ptr<VertexBuffer> GetVertexBuffer(GFX& gfx, float fov, float nearZ, float farZ, float startLength, float endLength, float aspectRatio, std::vector<D3D11_INPUT_ELEMENT_DESC>* layout);

	float CalculateLengthOfViewTriangle(float fov, float width);

	void UpdateVertexBuffer(GFX& gfx);

private:
	Camera* m_parent;
};


