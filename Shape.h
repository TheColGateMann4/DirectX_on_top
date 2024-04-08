#pragma once
#include "Includes.h"
#include "Graphics.h"
#include "RenderTechnique.h"

class Shape
{
	friend class SceneObject;
public:
	Shape() = default;
	Shape(const Shape&) = delete;
	virtual ~Shape() = default;

public:
	virtual void Render() const noexcept(!IS_DEBUG);
	void LinkToPipeline(class RenderGraph& renderGraph);

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
	
	template<class T>
	std::vector<T*> GetEveryBindableOfType(size_t techniqueNumber = 0, size_t stepNumber = 0) const noexcept
	{
		std::vector<T*> result;
		m_techniques.at(techniqueNumber).GetEveryBindableOfType<T>(stepNumber, result);

		return result;
	}

public:
	void SetTechniqueActive(size_t techniqueNum, bool active)
	{
		assert((m_techniques.size() - 1 >= techniqueNum) && "tried to get technique out of buffer");
		
		m_techniques.at(techniqueNum).m_active = active;

		for(auto& step : m_techniques.at(techniqueNum).m_steps)
			step.m_active = active;
	}

	bool GetTechniqueActive(size_t techniqueNum)
	{
		assert((m_techniques.size() - 1 >= techniqueNum) && "tried to get technique out of buffer");

		return m_techniques.at(techniqueNum).m_active;
	}

public:
	void AddRenderTechnique(const RenderTechnique& technique);

public:
	virtual DirectX::XMMATRIX GetTranformMatrix() const noexcept = 0;

	UINT32 GetIndexCount() const noexcept;

public:
	std::shared_ptr<class IndexBuffer> m_pIndexBuffer = nullptr;
	std::shared_ptr<class VertexBuffer> m_pVertexBuffer = nullptr;
	std::shared_ptr<class Topology> m_pTopology = nullptr;
	std::shared_ptr<class TransformConstBuffer> m_pTransformConstBuffer = nullptr;

private:
	std::vector<RenderTechnique> m_techniques = {};
};