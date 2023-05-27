#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(GFX& gfx, const std::wstring& path);
	VOID Bind(GFX& gfx) noexcept;
	ID3DBlob* GetByteCode() const noexcept;

protected:
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
};