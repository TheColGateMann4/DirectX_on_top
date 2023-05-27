#include "VertexShader.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(GFX& gfx, const std::wstring& path)
{
	HRESULT hr;

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			path.c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GetDevice(gfx)->CreateVertexShader
		(
			(VertexShader::pBlob->GetBufferPointer()),
			(VertexShader::pBlob->GetBufferSize()),
			NULL,
			&(VertexShader::pVertexShader)
		)
	);
}

VOID VertexShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, NULL);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return VertexShader::pBlob.Get();
}