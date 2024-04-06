#include "RenderGraph.h"
#include "RenderPass.h"
#include "ErrorMacros.h"
#include "Graphics.h"

//since we named everywhere RenderPassInput as input we can hardcode that into our macro
#define THROW_INPUT_ERROR(errorMessage) std::string errorString = errorMessage; errorString += input->GetLocalInfo();THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());

RenderGraph::RenderGraph(GFX& gfx)
	: m_backBuffer(gfx.GetRenderTarget()), m_depthStencilView(gfx.GetDepthStencil())
{
	AddGlobalOutput(RenderPassBufferOutput<RenderTarget>::GetUnique("backBuffer", m_backBuffer));
	AddGlobalOutput(RenderPassBufferOutput<DepthStencilView>::GetUnique("depthStencilView", m_depthStencilView));

	AddGlobalInput(RenderPassBufferInput<RenderTarget>::GetUnique("backBuffer", m_backBuffer));
}

void RenderGraph::Render(GFX& gfx) const noexcept(!IS_DEBUG)
{
	for (const auto& pass : m_passes)
		pass->Render(gfx);
}

void RenderGraph::Finish()
{
	assert(!m_finished);

	LinkGlobalInputs();

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

void RenderGraph::CaptureNextFrame()
{
	for (auto& pass : m_passes)
		if (RenderJobPass* jobPass = dynamic_cast<RenderJobPass*>(pass.get()))
			jobPass->CaptureNextFrame();
}

void RenderGraph::SetTarget(const char* inputName, const std::string& linkedResource)
{
	RenderPassInput* foundInput = nullptr;

	for (const auto& input : m_globalInputs)
		if (strcmp(input->GetInputName(), inputName) == 0)
			foundInput = input.get();

	if (foundInput == nullptr)
	{
		std::string errorMessage = "RenderPassInput could not be found. Ouput name was: \"";
		errorMessage += inputName;
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

	foundInput->Link(objectName, passName);
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

	LinkInputs(pass.get());

	m_passes.push_back(std::move(pass));
}

void RenderGraph::AddGlobalOutput(std::unique_ptr<RenderPassOutput> output)
{
	m_globalOutputs.push_back(std::move(output));
}

void RenderGraph::AddGlobalInput(std::unique_ptr<RenderPassInput> input)
{
	m_globalInputs.push_back(std::move(input));
}

std::unique_ptr<RenderPassOutput>* RenderGraph::GetGlobalOutput(const char* name)
{
	for (auto& output : m_globalOutputs)
		if (strcmp(output->GetName(), name) == 0)
			return &output;

	std::string errorMessage = "Global output could not be found. Searched name: ";
	errorMessage += name;
	errorMessage += "\".";

	THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());

	return nullptr;
}

std::unique_ptr<RenderPassInput>* RenderGraph::GetGlobalInput(const char* name)
{
	for (auto& input : m_globalInputs)
		if (strcmp(input->GetInputName(), name) == 0)
			return &input;

	std::string errorMessage = "Global input could not be found. Searched name: ";
	errorMessage += name;
	errorMessage += "\".";

	THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());

	return nullptr;
}

void RenderGraph::CheckGraphIntegrity()
{
	for (const auto& pass : m_passes)
		pass->CheckPassIntegrity();
}

void RenderGraph::LinkInputs(RenderPass* pass)
{
	for (std::unique_ptr<RenderPassInput>& input : pass->GetInputs())
	{
		if (input->GetLinkObjectOwnerName()[0] == '$')
		{
			bool found = false;
			for (const auto& globalOutput : m_globalOutputs)
			{
				if (input->GetLinkObjectName() == globalOutput->GetName())
				{
					found = true;
					input->Bind(*globalOutput);
					break;
				}
			}

			if(!found)
			{
				THROW_INPUT_ERROR("RenderPassOutput for RenderPassInput could not be found in RenderPass.");
			}
		}
		else
		{
			bool found = false;

			for (const auto& localPass : m_passes)
			{
				if(input->GetLinkObjectOwnerName() == localPass->GetName())
				{
					RenderPassOutput* output = localPass->GetOutput(input->GetLinkObjectName().c_str());

					if (output != nullptr)
					{
						found = true;
						input->Bind(*output);
						break;
					}
					else
					{
						THROW_INPUT_ERROR("RenderPassOutput for RenderPassInput could not be found in RenderPass.");
					}
				}
			}

			if (!found)
			{
				THROW_INPUT_ERROR("RenderPass for RenderPassInput could not be found.");
			}
		}
	}
}

void RenderGraph::LinkGlobalInputs()
{
	for (auto& input : m_globalInputs)
	{
		bool found = false;
		for (const auto& pass : m_passes)
		{
			if (pass->m_name == input->GetLinkObjectOwnerName())
			{
				RenderPassOutput* output = pass->GetOutput(input->GetLinkObjectName().c_str());

				if (output != nullptr)
				{
					found = true;
					input->Bind(*output);
					break;
				}
				else
				{
					THROW_INPUT_ERROR("RenderPassOutput for RenderPassInput could not be found in RenderPass.");
				}
			}
		}

		if (!found)
		{
			THROW_INPUT_ERROR("RenderPass for RenderPassInput could not be found.");
		}
	}
}

#undef THROW_INPUT_ERROR