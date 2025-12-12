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

public:
	void Bind(GFX& gfx) noexcept override;

	ID3D11Buffer* Get() const;

	UINT32 GetBufferByteSize() const;

	UINT32 GetBufferSize() const;

	const DynamicVertex::VertexLayout& GetLayout() const;

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
	UINT32 m_bufferSize;
	std::string m_bufferUID;
	DynamicVertex::VertexLayout m_vertexLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

