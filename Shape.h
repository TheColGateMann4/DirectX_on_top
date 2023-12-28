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
	// slotNumber is only used when trying to get contant buffer
	// techniqueNumber and stepNumber are defaulted since normal technique and normal step are always defined first - at least for now
	// isPixelShader is strictly for contant buffers, its defaulted to true since cached buffers for vertexShaders are very rare occurances
	template<class T>
	T* GetBindable(size_t techniqueNumber = 0, size_t stepNumber = 0, size_t slotNumber = 0, bool isPixelShader = true) const noexcept
	{
		return m_techniques.at(techniqueNumber).GetBindable<T>(stepNumber, slotNumber, isPixelShader);
	}

public:

	void SetTechniqueActive(size_t techniqueNum, size_t stepNum, bool active)
	{
		assert((m_techniques.size() - 1 >= techniqueNum) && "tried to get technique out of buffer");
		assert((m_techniques.at(techniqueNum).m_steps.size() - 1 >= stepNum) && "tried to get technique out of buffer");
		
		m_techniques.at(techniqueNum).m_steps.at(stepNum).m_active = active;
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