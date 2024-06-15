#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class ShaderUnorderedAccessView : public Bindable
{
public:
	ShaderUnorderedAccessView(GFX& gfx, UINT32 slot, Microsoft::WRL::ComPtr<ID3D11Resource> pResource, DXGI_FORMAT resourceFormat);
	ShaderUnorderedAccessView(GFX& gfx, UINT32 slot)
	{

	}
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<ShaderUnorderedAccessView> GetBindable(GFX& gfx, UINT32 slot)
	{
		return BindableList::GetBindable<ShaderUnorderedAccessView>(gfx, slot);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_slot);
	};

	static std::string GetStaticUID(const UINT32 slot) noexcept
	{
		return GenerateUID(slot);
	};

private:
	static std::string GenerateUID(const UINT32 slot)
	{
		return std::to_string(slot);
	}

	static constexpr D3D11_UAV_DIMENSION GetUAVDimension(const D3D11_RESOURCE_DIMENSION resourceDimension);

	static D3D11_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const Microsoft::WRL::ComPtr<ID3D11Resource>& pResource, const D3D11_RESOURCE_DIMENSION resourceDimension, const DXGI_FORMAT resourceFormat);

protected:
	UINT32 m_slot;
	DXGI_FORMAT m_resourceFormat;
	D3D11_UAV_DIMENSION m_resourceDimension;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_pResource;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUnorderedAccessView;
};
