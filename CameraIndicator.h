#pragma once
#include "Shape.h"

class CameraIndicator : public Shape
{
public:
	CameraIndicator(class GFX& gfx, Camera* parent);

public:
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

private:
	Camera* m_parent;
};

