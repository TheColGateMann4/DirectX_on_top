#include "VertexShader.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(GFX& gfx, const std::string& path)
{
	HRESULT hr;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			std::wstring(path.begin(), path.end()).c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GetDevice(gfx)->CreateVertexShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&pVertexShader
		)
	);
}

VertexShader::VertexShader(GFX& gfx, const std::string&& path)
{
	HRESULT hr;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			std::wstring(path.begin(), path.end()).c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GetDevice(gfx)->CreateVertexShader
		(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			NULL,
			&pVertexShader
		)
	);
}

VOID VertexShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, NULL);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBlob.Get();
}