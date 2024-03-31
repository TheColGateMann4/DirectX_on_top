#include "RenderGraph.h"
#include "RenderPass.h"
#include "ErrorMacros.h"
#include "Graphics.h"

//since we named everywhere RenderPassOutput as output we can hardcode that into our macro
#define THROW_OUTPUT_ERROR(errorMessage) std::string errorString = errorMessage; errorString += output->GetLocalInfo();THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());

RenderGraph::RenderGraph(GFX& gfx)
	: m_backBuffer(gfx.GetRenderTarget()), m_depthStencilView(gfx.GetDepthStencil())
{
	AddGlobalInput(RenderPassBufferInput<RenderTarget>::GetUnique("backBuffer", m_backBuffer));
	AddGlobalInput(RenderPassBufferInput<DepthStencilView>::GetUnique("depthStencilView", m_depthStencilView));

	AddGlobalOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("backBuffer", m_backBuffer));
}

void RenderGraph::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	for (const auto& pass : m_passes)
		pass->Render(gfx);
}

void RenderGraph::Finish()
{
	assert(!m_finished);

	LinkGlobalOutputs();

	for (const auto& pass : m_passes)
		pass->CheckPassIntegrity();

	m_finished = true;
}

void RenderGraph::Reset()
{
 	for (const auto& pass : m_passes)
 		pass->Reset();

 	m_finished = false;
}

void RenderGraph::SetTarget(const char* outputName, const std::string& linkedResource)
{
	RenderPassOutput* foundOutput = nullptr;

	for (const auto& output : m_globalOutputs)
		if (strcmp(output->GetOutputName(), outputName) == 0)
			foundOutput = output.get();

	if (foundOutput == nullptr)
	{
		std::string errorMessage = "RenderPassOutput could not be found. Ouput name was: \"";
		errorMessage += outputName;
		errorMessage += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());
	}

	size_t positionOfDotThatSeparates = linkedResource.find('.');

	if (positionOfDotThatSeparates == std::string::npos)
	{
		std::string errorString = "Invalid string structure was passed as resource symbol. Passed string was: \"";
		errorString += linkedResource;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}

	std::string passName = std::string(linkedResource.begin(), linkedResource.begin() + positionOfDotThatSeparates);
	std::string objectName = linkedResource.c_str() + positionOfDotThatSeparates + 1;

	foundOutput->Link(objectName, passName);
}

void RenderGraph::AddPass(std::unique_ptr<RenderPass> pass)
{
	for (size_t i = 0; i < m_passes.size(); i++)
		if (strcmp(pass->m_name, m_passes.at(i)->m_name) == 0)
		{
			std::string errorMessage = "Passed added had the same name as pass on position: ";
			errorMessage += '0' + i;
			errorMessage += ". Both passes had name: ";
			errorMessage += m_passes.at(i)->m_name;
			errorMessage += ".";

			THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());
		}

	LinkOutputs(pass.get());

	m_passes.push_back(std::move(pass));
}

void RenderGraph::AddGlobalInput(std::unique_ptr<RenderPassInput> input)
{
	m_globalInputs.push_back(std::move(input));
}

void RenderGraph::AddGlobalOutput(std::unique_ptr<RenderPassOutput> output)
{
	m_globalOutputs.push_back(std::move(output));
}

void RenderGraph::CheckGraphIntegrity()
{
	for (const auto& pass : m_passes)
		pass->CheckPassIntegrity();
}

void RenderGraph::LinkOutputs(RenderPass* pass)
{
	for (std::unique_ptr<RenderPassOutput>& output : pass->GetOutputs())
	{
		if (output->GetLinkObjectOwnerName()[0] == '$')
		{
			bool found = false;
			for (const auto& globalInput : m_globalInputs)
			{
				if (output->GetLinkObjectName() == globalInput->GetName())
				{
					found = true;
					output->Bind(*globalInput);
					break;
				}
			}

			if(!found)
			{
				THROW_OUTPUT_ERROR("RenderPassInput for RenderPassOutput could not be found in RenderPass.");
			}
		}
		else
		{
			bool found = false;

			for (const auto& localPass : m_passes)
			{
				if(output->GetLinkObjectOwnerName() == localPass->GetName())
				{
					RenderPassInput* input = localPass->GetInput(output->GetLinkObjectName().c_str());

					if (input != nullptr)
					{
						found = true;
						output->Bind(*input);
						break;
					}
					else
					{
						THROW_OUTPUT_ERROR("RenderPassInput for RenderPassOutput could not be found in RenderPass.");
					}
				}
			}

			if (!found)
			{
				THROW_OUTPUT_ERROR("RenderPass for RenderPassOutput could not be found.");
			}
		}
	}
}

void RenderGraph::LinkGlobalOutputs()
{
	for (auto& output : m_globalOutputs)
	{
		bool found = false;
		for (const auto& pass : m_passes)
		{
			if (pass->m_name == output->GetLinkObjectOwnerName())
			{
				RenderPassInput* input = pass->GetInput(output->GetLinkObjectName().c_str());

				if (input != nullptr)
				{
					found = true;
					output->Bind(*input);
					break;
				}
				else
				{
					THROW_OUTPUT_ERROR("RenderPassInput for RenderPassOutput could not be found in RenderPass.");
				}
			}
		}

		if (!found)
		{
			THROW_OUTPUT_ERROR("RenderPass for RenderPassOutput could not be found.");
		}
	}
}

#undef THROW_OUTPUT_ERROR