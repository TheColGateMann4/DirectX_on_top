#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(GFX& gfx, const std::vector<UINT32> indices);
	VOID Bind(GFX& gfx) noexcept override;
	UINT32 GetCount() const noexcept;

protected:
	UINT32 m_count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};

