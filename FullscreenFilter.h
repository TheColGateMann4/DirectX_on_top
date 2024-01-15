#pragma once
#include <memory>
#include <vector>
#include <string>

class FullscreenFilter
{
public:
	FullscreenFilter(class GFX& gfx);

	void ChangePixelShader(class GFX& gfx, std::string ShaderName);
	void ChangeBlurStrength(GFX& gfx, int strength);

	void Render(class GFX& gfx) const noexcept;

private:
	std::string currentShaderName = "normal";
	std::vector<std::shared_ptr<class Bindable>> m_bindables;
	class IndexBuffer* pIndexBuffer = nullptr;
};

