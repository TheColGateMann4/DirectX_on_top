#pragma once
#include "Includes.h"
#include "Bindable.h"
#include "BindableList.h"
#include "ErrorMacros.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(GFX& gfx, const DynamicVertex::VertexBuffer& vertexBuffer);

	VertexBuffer(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer);

	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<VertexBuffer> GetBindable(GFX& gfx, const std::string bufferUID, const DynamicVertex::VertexBuffer& vertexBuffer)
	{
		assert(bufferUID != "UNKNOWN");
		return BindableList::GetBindable<VertexBuffer>(gfx, bufferUID, vertexBuffer);
	}

	template <class ...Params>
	static std::string GetStaticUID(const std::string bufferUID, Params&&...)
	{
		return GenerateUID(bufferUID);
	};

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_bufferUID);
	};

private:
	static std::string GenerateUID(const std::string bufferUID)
	{
		return bufferUID;
	}

protected:
	UINT32 m_stride;
	std::string m_bufferUID;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

