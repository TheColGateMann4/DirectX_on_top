#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "RenderTechnique.h"

class Bindable;
class IndexBuffer;
class VertexBuffer;
class Topology;
class TransformConstBufferWithPixelShader;

class RenderQueue;

class Shape
{
public:
	Shape() = default;
	Shape(const Shape&) = delete;

public:
	virtual void Render(RenderQueue& renderQueue) const noexcept(!IS_DEBUG);

public:
	void Bind(GFX& gfx) const noexcept;

public:
	//slotNumber is only used when trying to get contant buffer
	//techniqueNumber and techniqueNumber are defaulted since normal technique and normal step are always defined first - at least for now
	template<class T>
	T* GetBindable(size_t techniqueNumber = 0, size_t stepNumber = 0, size_t slotNumber = 0) const noexcept
	{
		return m_techniques.at(techniqueNumber).GetBindable<T>(stepNumber, slotNumber);
	}

public:
	void SetIndexBuffer(std::shared_ptr<IndexBuffer>& pIndexBuffer)
	{
		m_pIndexBuffer = pIndexBuffer;
	}

public:
	void AddRenderTechnique(const RenderTechnique& technique);

public:
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept = 0;

	UINT32 GetIndexCount() const noexcept;

public:
	std::shared_ptr<IndexBuffer> m_pIndexBuffer = nullptr;
	std::shared_ptr<VertexBuffer> m_pVertexBuffer = nullptr;
	std::shared_ptr<Topology> m_pTopology = nullptr;
	std::shared_ptr<TransformConstBufferWithPixelShader> m_pTransformConstBuffer = nullptr;

private:
	std::vector<RenderTechnique> m_techniques = {};
};