#include "GaussBlurFilter.h"
#include "Includes.h"
#include "ConstantBuffers.h"
#include "Graphics.h"
#include "imgui/imgui.h"

GaussBlurFilter::GaussBlurFilter(GFX& gfx, int range, float sigma)
	: m_range(range), m_sigma(sigma)
{
	{
		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Int>("numberUsed");
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Array>("cooficients");
		constBufferLayout.SetArrayAttributes("cooficients", DynamicConstantBuffer::DataType::Float4, 15); // we gotta use float4 to use single float value in array because of hlsl alignment

		DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);

		m_cooficients = std::make_shared<CachedBuffer>(gfx, constBufferData, 0, true);
	}

	{
		DynamicConstantBuffer::BufferLayout constBufferLayout;
		constBufferLayout.Add<DynamicConstantBuffer::DataType::Bool>("horizontal");

		DynamicConstantBuffer::BufferData constBufferData(constBufferLayout);
		*constBufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = true;

		m_blurSettings = std::make_shared<CachedBuffer>(gfx, constBufferData, 1, true);
	}

	SetCooficients(gfx, range, sigma);
}

void GaussBlurFilter::Bind(GFX& gfx) const noexcept
{
	m_cooficients->Bind(gfx);
	m_blurSettings->Bind(gfx);
}

void GaussBlurFilter::SetCooficients(class GFX& gfx, int range, float sigma) noexcept
{
	assert(range <= 14);
	DynamicConstantBuffer::BufferData bufferData = m_cooficients->constBufferData;
	
	int* numberUsed = bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Int>("numberUsed");

	*numberUsed = range;

	float sum = 0.0f;
	for (int i = 0; i <= *numberUsed; i++)
	{
		const size_t cooficientIndex = std::abs(i - range);
		const float gaussDensity = CalculateGaussDensity(i - range, sigma);

		if (cooficientIndex > 0)
			sum += gaussDensity * 2;
		else
			sum += gaussDensity;

		float* cooficientValue = static_cast<float*>(bufferData.GetArrayDataPointerValue<DynamicConstantBuffer::DataType::Float4>("cooficients", cooficientIndex));
		*cooficientValue = gaussDensity;
	}

	for (int i = 0; i <= *numberUsed; i++)
	{
		float* cooficientValue = static_cast<float*>(bufferData.GetArrayDataPointerValue<DynamicConstantBuffer::DataType::Float4>("cooficients", i));

		*cooficientValue /= sum;
	}

	m_cooficients->Update(gfx, bufferData);
}

void GaussBlurFilter::SetHorizontal(GFX& gfx, bool horizontal)
{
	DynamicConstantBuffer::BufferData bufferData = m_blurSettings->constBufferData;
	*bufferData.GetElementPointerValue<DynamicConstantBuffer::DataType::Bool>("horizontal") = horizontal;
	m_blurSettings->Update(gfx, bufferData);
}

float GaussBlurFilter::CalculateGaussDensity(const float x, const float sigma) const noexcept
{
	const auto sigmaSquared = sigma * sigma;
	return (1.0f / std::sqrt(2.0f * _Pi * sigmaSquared)) * std::exp(-(x * x) / (2.0f * sigmaSquared));
}

void GaussBlurFilter::MakeImGuiPropeties(GFX& gfx)
{
	bool changed = false;

	auto checkChanged = [&changed](bool returnFromStatement) mutable
		{
			changed = changed || returnFromStatement;
		};

	checkChanged(ImGui::SliderInt("range", &m_range, 1, 14));
	checkChanged(ImGui::SliderFloat("sigma", &m_sigma, 1.0f, 20.0f));

	if (changed)
		SetCooficients(gfx, m_range, m_sigma);
}