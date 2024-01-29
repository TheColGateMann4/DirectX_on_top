#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include "BindableList.h"
#include "DynamicConstantBuffer.h"

class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer()
		:	
			pConstantBuffer(nullptr),
			m_slot(0),
			m_isPixelShader(false)
	{}

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, bool isPixelShader)
		: m_slot(slot),
		m_isPixelShader(isPixelShader)
	{
		HRESULT hr;

		bufferData.MakeFinished();

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = bufferData.GetSize();
		constBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA constBufferResourceData = {};
		constBufferResourceData.pSysMem = bufferData.GetBytes();

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, &constBufferResourceData, &pConstantBuffer));
	}

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout,UINT32 slot, bool isPixelShader)
		: m_slot(slot),
		m_isPixelShader(isPixelShader)
	{
		HRESULT hr;

		(void)layout.GetFinished();

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = DynamicConstantBuffer::BufferLayout::GetLayoutSize(layout);
		constBufferDesc.StructureByteStride = 0;

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, NULL, &pConstantBuffer));
	}


public:
	void Update(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData)
	{
		assert(pConstantBuffer != nullptr);

		HRESULT hr;

		D3D11_MAPPED_SUBRESOURCE subresourceData;

		THROW_GFX_IF_FAILED(
			GetDeviceContext(gfx)->Map(
				this->pConstantBuffer.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				NULL,
				&subresourceData)
		);
		memcpy(subresourceData.pData, bufferData.GetBytes(), bufferData.GetSize());

		GetDeviceContext(gfx)->Unmap(this->pConstantBuffer.Get(), NULL);
	}

	void Bind(GFX& gfx) noexcept override
	{
		assert(pConstantBuffer != nullptr);

		if (m_isPixelShader)
			GetDeviceContext(gfx)->PSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
		else
			GetDeviceContext(gfx)->VSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}

public:
	const ConstantBuffer& operator=(ConstantBuffer& toAssign)
	{
		if (pConstantBuffer != NULL)
			pConstantBuffer->Release();

		pConstantBuffer = toAssign.pConstantBuffer;
		m_slot = toAssign.m_slot;
		m_isPixelShader = toAssign.m_isPixelShader;

		return *this;
	}

public:
	UINT32 GetSlot() const
	{
		return m_slot;
	}

	bool isPixelShaderType() const
	{
		return m_isPixelShader;
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT32 m_slot;
	bool m_isPixelShader;
};




class CachedBuffer : public ConstantBuffer
{
public:
	CachedBuffer()
		:
		ConstantBuffer::ConstantBuffer()
	{}

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, bool isPixelShader)
		:
		constBufferData(bufferData),
		ConstantBuffer::ConstantBuffer(gfx, bufferData, slot, isPixelShader)
	{}

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, UINT32 slot, bool isPixelShader)
		:
		ConstantBuffer::ConstantBuffer(gfx, layout, slot, isPixelShader)
	{}

public:
	void Update(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData)
	{
		bufferData.MakeFinished();
		constBufferData = bufferData;

		ConstantBuffer::Update(gfx, bufferData);
	}

	using ConstantBuffer::Bind;

public:
	static std::shared_ptr<CachedBuffer> GetBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, bool isPixelShader)
	{
		return BindableList::GetBindable<CachedBuffer>(gfx, bufferData, slot, isPixelShader);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(constBufferData, m_slot, m_isPixelShader);
	};

	static std::string GetStaticUID(const DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, bool isPixelShader) noexcept
	{
		return GenerateUID(bufferData, slot, isPixelShader);
	};

private:
	static std::string GenerateUID(const DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, bool isPixelShader)
	{
		return bufferData.GetLayout().GetIdentificator() + '@' + std::to_string(slot) + '@' + (char)('0' + isPixelShader);
	}

public:
	const CachedBuffer& operator=(CachedBuffer& toAssign)
	{
		constBufferData = toAssign.constBufferData;

		if(pConstantBuffer != NULL)
			pConstantBuffer->Release();

		pConstantBuffer = toAssign.pConstantBuffer;
		m_slot = toAssign.m_slot;
		m_isPixelShader = toAssign.m_isPixelShader;

		return *this;
	}

public:
	DynamicConstantBuffer::BufferData constBufferData;

private:
	using ConstantBuffer::pConstantBuffer;
	using ConstantBuffer::m_slot;
	using ConstantBuffer::m_isPixelShader;
};





class NonCachedBuffer : public ConstantBuffer
{
public:
	using ConstantBuffer::ConstantBuffer;

public:
	using ConstantBuffer::Update;
	using ConstantBuffer::Bind;

public:
	const NonCachedBuffer& operator=(NonCachedBuffer toAssign) // made specially for one case, thats why non const and by value, might be changed for a future
	{
		if (pConstantBuffer != NULL)
			pConstantBuffer->Release();

		pConstantBuffer = toAssign.pConstantBuffer;
		m_slot = toAssign.m_slot;
		m_isPixelShader = toAssign.m_isPixelShader;

		return *this;
	}

private:
	using ConstantBuffer::pConstantBuffer;
	using ConstantBuffer::m_slot;
	using ConstantBuffer::m_isPixelShader;
};