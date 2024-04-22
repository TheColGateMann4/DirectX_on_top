#pragma once
#include "Includes.h"
#include "RenderPass.h"
#include "RenderJobPass.h"
#include "RenderPassOuput.h"
#include "RenderPassInput.h"
#include "RenderTarget.h"
#include "DepthStencilView.h"

class RenderGraph
{
public:
	RenderGraph(class GFX& gfx);

public:
	void Render(class GFX& gfx) const noexcept(!IS_DEBUG);
	void Finish();

	void Reset();

public:
	void CaptureNextFrame();

	void SetTarget(const char* inputName, const std::string& linkedResource);

	void SetPassesScenePtr(class Scene* scene);

	void AddPass(std::unique_ptr<RenderPass> pass);

	void AddGlobalOutput(std::unique_ptr<RenderPassOutput> output);
	void AddGlobalInput(std::unique_ptr<RenderPassInput> input);

	std::unique_ptr<RenderPassOutput>* GetGlobalOutput(const char* name);
	std::unique_ptr<RenderPassInput>* GetGlobalInput(const char* name);

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
	void LinkInputs(RenderPass* pass);
	void LinkGlobalInputs();

protected:
	std::vector<std::unique_ptr<RenderPass>> m_passes;
	std::vector<std::unique_ptr<RenderPassOutput>> m_globalOutputs;
	std::vector<std::unique_ptr<RenderPassInput>> m_globalInputs;

	std::shared_ptr<RenderTarget>* m_backBuffer;
	std::shared_ptr<DepthStencilView>* m_depthStencilView;

	bool m_finished = false;
};

