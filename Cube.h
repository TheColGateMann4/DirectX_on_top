#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "DynamicConstantBuffer.h"
#include <random>

class Cube : public Shape
{
public:
	Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, bool enableOutline);

	std::vector<std::shared_ptr<Bindable>> getOutline(GFX& gfx);

public:
	void DrawWithOutline(GFX& gfx) const noexcept(!IS_DEBUG);

public:
	void ResetLocalTransform() noexcept;
	void SpawnControlWindow(GFX& gfx) noexcept;

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;

private:
	static SimpleMesh GetNormalMesh(float scale);
	static SimpleMesh GetUnwrappedMesh(float scale, bool getExtendedStuff = false);

private:
	DirectX::XMFLOAT3 m_position = {};
	DirectX::XMFLOAT3 m_rotation = {};
	DirectX::XMFLOAT3 m_scale = {1.0f, 1.0f, 1.0f};

private:
	DynamicConstantBuffer::BufferData shaderMaterial;
	bool materialsDefined = false;

private:
	bool m_glowEnabled = false;
	std::vector<std::shared_ptr<Bindable>> m_outlineBindables = {};
	const class IndexBuffer* m_pOutlineIndexBuffer = nullptr;
};