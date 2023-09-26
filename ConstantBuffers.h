#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include "BindableList.h"

template<class C>
class ConstantBuffer : public Bindable
{
public:
	void Update(GFX& gfx, const C& consts)
	{
		constBufferData = consts;
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
		constBufferData = consts;
		HRESULT hr;

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = sizeof(consts);
		constBufferDesc.StructureByteStride = 0;

		m_bufferSize = sizeof(consts);

		D3D11_SUBRESOURCE_DATA constBufferResourceData = {};
		constBufferResourceData.pSysMem = &consts;

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, &constBufferResourceData, &(this->pConstantBuffer)));
	}
	ConstantBuffer(GFX& gfx, UINT32 slot)
		: m_slot(slot)
	{
		constBufferData = {};
		HRESULT hr;

		D3D11_BUFFER_DESC constBufferDesc = {};
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = NULL;
		constBufferDesc.ByteWidth = sizeof(C);
		constBufferDesc.StructureByteStride = 0;

		m_bufferSize = sizeof(C);

		THROW_GFX_IF_FAILED(GetDevice(gfx)->CreateBuffer(&constBufferDesc, NULL, &(this->pConstantBuffer)));
	}

public:
	C constBufferData;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT32 m_slot;
	UINT32 m_bufferSize;
};

template<class C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetDeviceContext;
	using ConstantBuffer<C>::pConstantBuffer;
	using ConstantBuffer<C>::m_slot;
	using ConstantBuffer<C>::m_bufferSize;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	using ConstantBuffer<C>::constBufferData;

	void Bind(GFX& gfx) noexcept override
	{
		GetDeviceContext(gfx)->PSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}

public:
	static std::shared_ptr<PixelConstantBuffer> GetBindable(GFX& gfx, const C& consts, UINT32 slot)
	{
		return BindableList::GetBindable<PixelConstantBuffer>(gfx, consts, slot);
	}
	static std::shared_ptr<PixelConstantBuffer> GetBindable(GFX& gfx, UINT32 slot)
	{
		return BindableList::GetBindable<PixelConstantBuffer>(gfx, slot);
	}

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_slot);
	};

	static std::string GetUID(const C& consts, UINT32 slot) noexcept
	{
		return GenerateUID(slot);
	};

private:
	static std::string GenerateUID(const C& consts, UINT32 slot)
	{
		GenerateUID(slot);
	}
	static std::string GenerateUID(UINT32 slot)
	{
		std::string result;
		result += typeid(PixelConstantBuffer).name();
		result += "@";
		result += typeid(C).name();
		result += "@";
		result += std::to_string(slot);

		return result;
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

	void Bind(GFX& gfx) noexcept override
	{
		GetDeviceContext(gfx)->VSSetConstantBuffers(m_slot, 1, pConstantBuffer.GetAddressOf());
	}

public:
	static std::shared_ptr<VertexConstantBuffer> GetBindable(GFX& gfx, const C& consts, UINT32 slot)
	{
		return BindableList::GetBindable<VertexConstantBuffer>(gfx, consts, slot);
	}
	static std::shared_ptr<VertexConstantBuffer> GetBindable(GFX& gfx, UINT32 slot)
	{
		return BindableList::GetBindable<VertexConstantBuffer>(gfx, slot);
	}

	std::string GetUID() const noexcept override
	{
		return GenerateUID(m_slot);
	};

private:
	static std::string GenerateUID(const C& consts, UINT32 slot)
	{
		GenerateUID(slot);
	}
	static std::string GenerateUID(UINT32 slot)
	{
		std::string result;
		result += typeid(PixelConstantBuffer).name();
		result += "@";
		result += typeid(C).name();
		result += "@";
		result += std::to_string(slot);

		return result;
	}
};