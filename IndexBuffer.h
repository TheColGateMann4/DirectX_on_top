#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include <wrl.h>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(GFX& gfx, const std::vector<UINT32>& indices)
		:	IndexBuffer(gfx, "UNKNOWN", indices){}

	IndexBuffer(GFX& gfx, const std::string bufferUID, const std::vector<UINT32>& indices);

	VOID Bind(GFX& gfx) noexcept override;

	UINT32 GetCount() const noexcept;

public:
	static std::shared_ptr<IndexBuffer> GetBindable(GFX& gfx, const std::string bufferUID, const std::vector<UINT32>& indices)
	{
		assert(bufferUID != "UNKNOWN");
		return BindableList::GetBindable<IndexBuffer>(gfx, bufferUID, indices);
	}

	template <class ...Params>
	static std::string GetStaticUID(const std::string bufferUID, Params&&...)
	{
		return GenerateUID(bufferUID);
	};

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_bufferUID);
	};

private:
	static std::string GenerateUID(const std::string bufferUID)
	{
		return bufferUID;
	}

protected:
	std::string m_bufferUID;
	UINT32 m_count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
};

