#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class ShaderUnorderedAccessView : public Bindable
{
public:
	ShaderUnorderedAccessView(GFX& gfx, UINT32 slot, Microsoft::WRL::ComPtr<ID3D11Resource> pResource, DXGI_FORMAT resourceFormat);

	void Bind(GFX& gfx) noexcept override;

public:
	ID3D11Resource* GetResource() const;

public:
	static std::shared_ptr<ShaderUnorderedAccessView> GetBindable(GFX& gfx, UINT32 slot, Microsoft::WRL::ComPtr<ID3D11Resource> pResource, DXGI_FORMAT resourceFormat)
	{
		return std::make_shared<ShaderUnorderedAccessView>(gfx, slot, pResource, resourceFormat);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID();
	};

	static std::string GetStaticUID() noexcept
	{
		return GenerateUID();
	};

private:
	static std::string GenerateUID()
	{
		return "";
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
