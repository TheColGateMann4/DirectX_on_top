#include "VertexShader.h"
#include "ErrorMacros.h"
#include "StringConverting.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(GFX& gfx, std::string path)
	:
	m_path(path)
{
	HRESULT hr;

	m_path = "Shaders\\" + m_path;
	std::wstring wpath = StringConverting::NarrowToWide(m_path);

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			wpath.c_str(), &pBlob
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

void VertexShader::Bind(GFX& gfx) noexcept
{
	GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, NULL);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBlob.Get();
}