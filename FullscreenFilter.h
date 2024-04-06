#pragma once
#include <memory>
#include <vector>
#include <string>
#include "GaussBlurFilter.h"

class FullscreenFilter
{
public:
	FullscreenFilter(class GFX& gfx);

	void ChangePixelShader(class GFX& gfx, std::string ShaderName);
	void ChangeBlurStrength(class GFX& gfx, int strength);

	void Bind(class GFX& gfx) const noexcept;
	void Draw(class GFX& gfx) const;

	void BindGaussBlur(class GFX& gfx);

	GaussBlurFilter* GetGuassBlurFilter();

private:
	std::string currentShaderName = "normal";
	std::vector<std::shared_ptr<class Bindable>> m_bindables;
	std::shared_ptr<class CachedBuffer> m_constBuffer;
	class IndexBuffer* pIndexBuffer;
	GaussBlurFilter m_gaussFilter;
};

