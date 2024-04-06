#pragma once
#include "Includes.h"

class Bindable
{
public:
	virtual void Bind(class GFX& gfx) noexcept = 0;
	virtual ~Bindable() = default;

	virtual std::string GetLocalUID() const noexcept { return ""; };
};

