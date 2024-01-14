#pragma once
#include "ErrorMacros.h"
#include "includes.h"
#include "Graphics.h"

class Bindable : public GraphicResource
{
public:
	virtual void Bind(GFX& gfx) noexcept = 0;
	virtual ~Bindable() = default;

	virtual std::string GetLocalUID() const noexcept { return ""; };
};

