#pragma once
#include "Shape.h"
#include "Graphics.h"
#include "SimpleMesh.h"
#include "DynamicConstantBuffer.h"
#include "Mesh.h"
#include "SceneObject.h"
#include <random>

class Cube : public SceneObject
{
public:
	Cube(GFX& gfx, float scale, std::string diffuseTexture, std::string normalTexture, bool enableOutline);

	std::vector<std::shared_ptr<Bindable>> getOutline(GFX& gfx);

public:
	void Draw(GFX& gfx) const noexcept(!IS_DEBUG) override;
	void DrawWithOutline(GFX& gfx) const noexcept(!IS_DEBUG);

public:
	void ResetLocalTransform() noexcept;
	void MakePropeties(GFX& gfx, float deltaTime) override;

public:
	std::string GetName() const override
	{
		return "Cube";
	}

public:
	DirectX::XMMATRIX GetTranformMatrix() const noexcept;

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

private:
	std::unique_ptr<Mesh> m_mesh;
};