#include "PixelShader.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(GFX& gfx, std::string path)
	: m_path(path)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	path = "Shaders\\" + path;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			std::wstring(path.begin(), path.end()).c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GetDevice(gfx)->CreatePixelShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&pPixelShader
		)
	);
}

VOID PixelShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->PSSetShader(pPixelShader.Get(), NULL, NULL);
}