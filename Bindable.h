#pragma once
#include "Includes.h"

enum TargetShader
{
	TargetPixelShader,
	TargetVertexShader,
	TargetHullShader,
	TargetDomainShader
};

struct TargetShaderBufferBinding
{
	TargetShader type = TargetVertexShader;
	UINT32 slot = 0;
};

class Bindable
{
public:
	virtual void Bind(class GFX& gfx) noexcept = 0;
	virtual ~Bindable() = default;

	virtual std::string GetLocalUID() const noexcept { return ""; };
};

