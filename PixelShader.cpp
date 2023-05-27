#include "PixelShader.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(GFX& gfx, const std::wstring& path)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			path.c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GetDevice(gfx)->CreatePixelShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&(PixelShader::pPixelShader)
		)
	);
}

VOID PixelShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetShader(pPixelShader.Get(), NULL, NULL);
}