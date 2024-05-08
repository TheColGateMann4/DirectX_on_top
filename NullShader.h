#pragma once
#include "Bindable.h"
#include "BindableList.h"
#include "wrl_no_warnings.h"

class NullShader : public Bindable
{
public:
	NullShader(GFX& gfx, TargetShader targetShader);
	void Bind(GFX& gfx) noexcept override;

public:
	static std::shared_ptr<NullShader> GetBindable(GFX& gfx, TargetShader targetShader)
	{
		return BindableList::GetBindable<NullShader>(gfx, targetShader);
	}

	std::string GetLocalUID() const noexcept override
	{
		return GenerateUID(m_targetShader);
	};

	static std::string GetStaticUID(TargetShader targetShader) noexcept
	{
		return GenerateUID(targetShader);
	};

private:
	static std::string GenerateUID(TargetShader targetShader)
	{
		return std::to_string(targetShader);
	}

protected:
	TargetShader m_targetShader;
};
