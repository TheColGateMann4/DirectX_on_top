#include "ConstantBuffers.h"


ConstantBuffer::ConstantBuffer()
	:
	pConstantBuffer(nullptr),
	m_targetShaders({ {TargetPixelShader, 0} }) // if light buffer will be overriden during runtime we will notice immediately
{

}

ConstantBuffer::ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders)
	:
	m_targetShaders(targetShaders)
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

ConstantBuffer::ConstantBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, std::vector<TargetShaderBufferBinding> targetShader)
	:
	m_targetShaders(targetShader)
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


void ConstantBuffer::Update(GFX& gfx, const DynamicConstantBuffer::BufferData& bufferData)
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

void ConstantBuffer::Bind(GFX& gfx) noexcept
{
	for (const auto& targetShader : m_targetShaders)
	{
		switch (targetShader.type)
		{
			case TargetPixelShader:
			{
				GFX::GetDeviceContext(gfx)->PSSetConstantBuffers(targetShader.slot, 1, pConstantBuffer.GetAddressOf());
				break;
			}
			case TargetVertexShader:
			{
				GFX::GetDeviceContext(gfx)->VSSetConstantBuffers(targetShader.slot, 1, pConstantBuffer.GetAddressOf());
				break;
			}
			case TargetHullShader:
			{
				GFX::GetDeviceContext(gfx)->HSSetConstantBuffers(targetShader.slot, 1, pConstantBuffer.GetAddressOf());
				break;
			}
			case TargetDomainShader:
			{
				GFX::GetDeviceContext(gfx)->DSSetConstantBuffers(targetShader.slot, 1, pConstantBuffer.GetAddressOf());
				break;
			}
			case TargetComputeShader:
			{
				GFX::GetDeviceContext(gfx)->CSSetConstantBuffers(targetShader.slot, 1, pConstantBuffer.GetAddressOf());
				break;
			}
		}
	}
}

const ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer& toAssign)
{
	if (pConstantBuffer != NULL)
		pConstantBuffer->Release();

	pConstantBuffer = toAssign.pConstantBuffer;
	m_targetShaders = toAssign.m_targetShaders;

	return *this;
}

std::vector<TargetShaderBufferBinding> ConstantBuffer::GetShaders() const
{
	return m_targetShaders;
}





CachedBuffer::CachedBuffer()
	:
	ConstantBuffer::ConstantBuffer()
{}

CachedBuffer::CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders)
	:
	constBufferData(bufferData),
	ConstantBuffer::ConstantBuffer(gfx, bufferData, targetShaders)
{}

CachedBuffer::CachedBuffer(GFX& gfx, DynamicConstantBuffer::BufferLayout& layout, std::vector<TargetShaderBufferBinding> targetShaders)
	:
	ConstantBuffer::ConstantBuffer(gfx, layout, targetShaders)
{}

DynamicConstantBuffer::BufferData CachedBuffer::GetBufferData()
{
	return constBufferData;
}

void CachedBuffer::Update(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData)
{
	bufferData.MakeFinished();
	constBufferData = bufferData;

	ConstantBuffer::Update(gfx, bufferData);
}

void CachedBuffer::SetBufferID(const char* bufferID)
{
	m_bufferID = bufferID;
}

std::shared_ptr<CachedBuffer> CachedBuffer::GetBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders)
{
	return std::make_shared<CachedBuffer>(gfx, bufferData, targetShaders);
}

std::shared_ptr<CachedBuffer> CachedBuffer::GetSharedBindable(GFX& gfx, DynamicConstantBuffer::BufferData& bufferData, std::vector<TargetShaderBufferBinding> targetShaders, const char* bufferID)
{
	std::shared_ptr<CachedBuffer> pCachedBuffer = std::make_shared<CachedBuffer>(gfx, bufferData, targetShaders);
	pCachedBuffer->m_bufferID = bufferID;

	BindableList::PushBindable(pCachedBuffer, bufferID);

	return pCachedBuffer;
}

std::shared_ptr<CachedBuffer> CachedBuffer::GetBindableWithoutCreation(GFX& gfx, const char* bufferID)
{
	return BindableList::GetBindableWithoutCreation<CachedBuffer>(gfx, bufferID);
}

std::string CachedBuffer::GetLocalUID() const noexcept
{
	return GenerateUID(m_bufferID.c_str());
};

std::string CachedBuffer::GetStaticUID(const char* bufferID) noexcept
{
	return GenerateUID(bufferID);
};

std::string CachedBuffer::GenerateUID(const char* bufferID)
{
	return bufferID;
}




std::shared_ptr<NonCachedBuffer> NonCachedBuffer::GetBindable(GFX& gfx, DynamicConstantBuffer::BufferLayout& bufferLayout, std::vector<TargetShaderBufferBinding> targetShaders)
{
	return std::make_shared<NonCachedBuffer>(gfx, bufferLayout, targetShaders);
}

NonCachedBuffer& NonCachedBuffer::operator=(NonCachedBuffer toAssign)
{
	if (pConstantBuffer != NULL)
		pConstantBuffer->Release();

	pConstantBuffer = toAssign.pConstantBuffer;
	m_targetShaders = toAssign.m_targetShaders;

	return *this;
}