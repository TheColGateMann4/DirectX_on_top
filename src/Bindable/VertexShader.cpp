#include "VertexShader.h"
#include "ErrorMacros.h"
#include "StringConverting.h"
#include "Graphics.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(GFX& gfx, std::string path)
	:
	m_path(path)
{
	HRESULT hr;

#ifdef _DEBUG
	path = "build\\Shaders\\" + path;
#else
	path = "Shaders\\" + path;
#endif

	std::wstring wpath = StringConverting::NarrowToWide(path);

	THROW_GFX_IF_FAILED
	(
		D3DReadFileToBlob
		(
			wpath.c_str(), &pBlob
		)
	);

	THROW_GFX_IF_FAILED
	(
		GFX::GetDevice(gfx)->CreateVertexShader
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
	GFX::GetDeviceContext(gfx)->VSSetShader(pVertexShader.Get(), NULL, NULL);
}

ID3DBlob* VertexShader::GetByteCode() const noexcept
{
	return pBlob.Get();
}