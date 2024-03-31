#pragma once
#include "Includes.h"
#include "RenderPass.h"
#include "RenderJobPass.h"
#include "RenderPassInput.h"
#include "RenderPassOutput.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"

class RenderGraph
{
public:
	RenderGraph(class GFX& gfx);

public:
	void Render(class GFX& gfx) const noexcept(!_DEBUG);
	void Finish();

	void Reset();

public:
	void SetTarget(const char* outputName, const std::string& linkedResource);

	void AddPass(std::unique_ptr<RenderPass> pass);

	void AddGlobalInput(std::unique_ptr<RenderPassInput> input);
	void AddGlobalOutput(std::unique_ptr<RenderPassOutput> output);

	void CheckGraphIntegrity();

public:
	RenderJobPass* GetRenderJobPass(const char* passName)
	{
		for (const auto& pass : m_passes)
			if (strcmp(pass->m_name, passName) == 0)
				if(RenderJobPass* resultPass = dynamic_cast<RenderJobPass*>(pass.get()))
				return resultPass;

		std::string errorString = "RenderPass could not be found. RenderPass name was: \"";
		errorString += passName;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());

		return nullptr;
	}

private:
	void LinkOutputs(RenderPass* pass);
	void LinkGlobalOutputs();

private:
	std::vector<std::unique_ptr<RenderPass>> m_passes;
	std::vector<std::unique_ptr<RenderPassInput>> m_globalInputs;
	std::vector<std::unique_ptr<RenderPassOutput>> m_globalOutputs;

	std::shared_ptr<RenderTarget>* m_backBuffer;
	std::shared_ptr<DepthStencilView>* m_depthStencilView;

	bool m_finished = false;
};

