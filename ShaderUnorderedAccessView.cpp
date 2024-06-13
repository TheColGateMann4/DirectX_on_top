#include "ShaderUnorderedAccessView.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <DirectXTex.h> // DirectX::BitsPerPixel

ShaderUnorderedAccessView::ShaderUnorderedAccessView(GFX& gfx, UINT32 slot, Microsoft::WRL::ComPtr<ID3D11Resource> pResource, DXGI_FORMAT resourceFormat, D3D11_UAV_DIMENSION resourceDimension)
	:
	m_slot(slot),
	m_resourceFormat(resourceFormat),
	m_resourceDimension(resourceDimension)
{
	HRESULT hr;

	pResource.CopyTo(&m_pResource);


	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

	m_pResource->QueryInterface(pBuffer.GetAddressOf());
	
	D3D11_BUFFER_DESC bufferDesc;

	pBuffer->GetDesc(&bufferDesc);

	assert(bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS && "Buffer wasn't created with D3D11_BIND_UNORDERED_ACCESS flag");

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = resourceFormat;
	uavDesc.ViewDimension = resourceDimension;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = bufferDesc.ByteWidth / (DirectX::BitsPerPixel(resourceFormat) / 8);
	uavDesc.Buffer.Flags = NULL;

	THROW_GFX_IF_FAILED
	(
		GFX::GetDevice(gfx)->CreateUnorderedAccessView
		(
			pBuffer.Get(),
			&uavDesc,
			&pUnorderedAccessView
		)
	);
}

VOID ShaderUnorderedAccessView::Bind(GFX& gfx) noexcept
{
	GFX::GetDeviceContext(gfx)->CSSetUnorderedAccessViews(m_slot, 1, pUnorderedAccessView.GetAddressOf(), NULL);
}