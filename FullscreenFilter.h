#pragma once
#include <memory>
#include <vector>

class FullscreenFilter
{
public:
	FullscreenFilter(class GFX& gfx);

	void Render(class GFX& gfx) const noexcept;

private:
	std::vector<std::shared_ptr<class Bindable>> m_bindables;
	class IndexBuffer* pIndexBuffer = nullptr;
};

