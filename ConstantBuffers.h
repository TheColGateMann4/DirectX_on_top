#pragma once
#include "Bindable.h"

template<class C>
class ConstantBuffer : public Bindable
{
public:
	void Update(GFX& gfx, const C& consts)
	{
		HRESULT hr;

		D3D11_MAPPED_SUBRESOURCE subresourceData;

		THROW_GFX_IF_FAILED(
			GetDeviceContext(gfx)->Map(
				pConstantBuffer.Get(),
				NULL,
				D3D11_MAP_WRITE_DISCARD,
				NULL,
				&subresourceData)
		);
		memcpy(subresourceData.pData, &consts, sizeof(consts));

		GetDeviceContext(gfx)->Unmap(pConstantBuffer.Get(), NULL);
	}
	ConstantBuffer(GFX& gfx, const C& consts, UINT32 slot)
		: m_slot(slot)
	{
		HRESULT hr;

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = sizeof(consts);
		constBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA constBufferResourceData = {};
		constBufferResourceData.pSysMem = &consts;

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, &constBufferResourceData, &(this->pConstantBuffer)));
	}
	ConstantBuffer(GFX& gfx, UINT32 slot)
		: m_slot(slot)
	{
		HRESULT hr;

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = sizeof(C);
		constBufferDesc.StructureByteStride = 0;

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, NULL, &(this->pConstantBuffer)));
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT32 m_slot;
};

template<class C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetDeviceContext;
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::m_slot;
public:
	using ConstantBuffer<C>::ConstantBuffer;

	VOID Bind(GFX& gfx) noexcept override
	{
		GetDeviceContext(gfx)->PSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}
};

template<class C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetDeviceContext;
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::m_slot;
public:
	using ConstantBuffer<C>::ConstantBuffer;

	VOID Bind(GFX& gfx) noexcept override
	{
		GetDeviceContext(gfx)->VSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}
};