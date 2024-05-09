#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include "ErrorMacros.h"
#include "BindableList.h"
#include "DynamicConstantBuffer.h"

class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer()
		:	
			pConstantBuffer(nullptr),
			m_slot(0),
			m_targetShader(TargetPixelShader)
	{}

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, TargetShader targetShader)
		: m_slot(slot),
		m_targetShader(targetShader)
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

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&constBufferDesc, &constBufferResourceData, &pConstantBuffer));
	}

	ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout,UINT32 slot, TargetShader targetShader)
		: m_slot(slot),
		m_targetShader(targetShader)
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

		THROW_GFX_IF_FAILED(GFX::GetDevice(gfx)->CreateBuffer(&constBufferDesc, NULL, &pConstantBuffer));
	}


public:
	void Update(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData)
	{
		assert(pConstantBuffer != nullptr);

		HRESULT hr;

		D3D11_MAPPED_SUBRESOURCE subresourceData;

		THROW_GFX_IF_FAILED(
			GFX::GetDeviceContext(gfx)->Map(
				this->pConstantBuffer.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				NULL,
				&subresourceData)
		);
		memcpy(subresourceData.pData, bufferData.GetBytes(), bufferData.GetSize());

		GFX::GetDeviceContext(gfx)->Unmap(this->pConstantBuffer.Get(), NULL);
	}

	void Bind(GFX& gfx) noexcept override
	{
		if (m_targetShader == TargetPixelShader)
			GFX::GetDeviceContext(gfx)->PSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
		else if (m_targetShader == TargetVertexShader)
			GFX::GetDeviceContext(gfx)->VSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
		else if (m_targetShader == TargetHullShader)
			GFX::GetDeviceContext(gfx)->HSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
		else if (m_targetShader == TargetDomainShader)
			GFX::GetDeviceContext(gfx)->DSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}

public:
	const ConstantBuffer& operator=(ConstantBuffer& toAssign)
	{
		if (pConstantBuffer != NULL)
			pConstantBuffer->Release();

		pConstantBuffer = toAssign.pConstantBuffer;
		m_slot = toAssign.m_slot;
		m_targetShader = toAssign.m_targetShader;

		return *this;
	}

public:
	UINT32 GetSlot() const
	{
		return m_slot;
	}

	TargetShader GetShaderType() const
	{
		return m_targetShader;
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT32 m_slot;
	TargetShader m_targetShader;
};




class CachedBuffer : public ConstantBuffer
{
public:
	CachedBuffer()
		:
		ConstantBuffer::ConstantBuffer()
	{}

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, TargetShader targetShader)
		:
		constBufferData(bufferData),
		ConstantBuffer::ConstantBuffer(gfx, bufferData, slot, targetShader)
	{}

	CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, UINT32 slot, TargetShader targetShader)
		:
		ConstantBuffer::ConstantBuffer(gfx, layout, slot, targetShader)
	{}

public:
	DynamicConstantBuffer::BufferData GetBufferData()
	{
		return constBufferData;
	}

	void Update(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData)
	{
		bufferData.MakeFinished();
		constBufferData = bufferData;

		ConstantBuffer::Update(gfx, bufferData);
	}

	using ConstantBuffer::Bind;

	void SetBufferID(const char* bufferID)
	{
		m_bufferID = bufferID;
	}

public:
	static std::shared_ptr<CachedBuffer> GetBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, UINT32 slot, TargetShader targetShader, const char* bufferID)
	{
		std::shared_ptr<CachedBuffer> pCachedBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, slot, targetShader);
		pCachedBuffer->m_bufferID = bufferID;

		BindableList::PushBindable(pCachedBuffer, bufferID);

		return pCachedBuffer;
	}

	static std::shared_ptr<CachedBuffer> GetBindableWithoutCreation(GFX& gfx, const char* bufferID)
	{
		return BindableList::GetBindableWithoutCreation<CachedBuffer>(gfx, bufferID);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_bufferID.c_str());
	};

	static std::string GetStaticUID(const char* bufferID) noexcept
	{
		return GenerateUID(bufferID);
	};

private:
	static std::string GenerateUID(const char* bufferID)
	{
		return bufferID;
	}

public:
	const CachedBuffer& operator=(CachedBuffer& toAssign)
	{
		constBufferData = toAssign.constBufferData;

		if(pConstantBuffer != NULL)
			pConstantBuffer->Release();

		pConstantBuffer = toAssign.pConstantBuffer;
		m_slot = toAssign.m_slot;
		m_targetShader = toAssign.m_targetShader;

		return *this;
	}

public:
	DynamicConstantBuffer::BufferData constBufferData;

private:
	using ConstantBuffer::pConstantBuffer;
	using ConstantBuffer::m_slot;
	using ConstantBuffer::m_targetShader;
	std::string m_bufferID = "";
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
		m_targetShader = toAssign.m_targetShader;

		return *this;
	}

private:
	using ConstantBuffer::pConstantBuffer;
	using ConstantBuffer::m_slot;
	using ConstantBuffer::m_targetShader;
};