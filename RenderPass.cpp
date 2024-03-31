#include "RenderPass.h"
#include "ErrorMacros.h"

RenderPass::RenderPass(const char* name)
	: m_name(name)
{

}

const char* RenderPass::GetName() const
{
	return m_name;
}

RenderPassInput* RenderPass::GetInput(const char* inputName)
{
	for (const auto& input : m_inputs)
	{
		if (strcmp(input->GetName(), inputName) == 0)
			return input.get();
	}

	std::string errorString = "RenderPassIntput could not be found in RenderPass. intput name was: \"";
	errorString += inputName;
	errorString += "\". RenderPass name was: \"";
	errorString += this->m_name;
	errorString += "\".";

	return nullptr;
}

RenderPassOutput* RenderPass::GetOutput(const char* outputName)
{
	for (const auto& output : m_outputs)
	{
		if (strcmp(output->GetOutputName(), outputName) == 0)
			return output.get();
	}

	std::string errorString = "RenderPassOutput could not be found in RenderPass. output name was: \"";
	errorString += outputName;
	errorString += "\". RenderPass name was: \"";
	errorString += this->m_name;
	errorString += "\".";

	THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());

	return nullptr;
}

std::vector<std::unique_ptr<RenderPassOutput>>& RenderPass::GetOutputs()
{
	return m_outputs;
}

void RenderPass::RegisterInput(std::unique_ptr<RenderPassInput> renderInput)
{
	m_inputs.push_back(std::move(renderInput));
}

void RenderPass::RegisterOutput(std::unique_ptr<RenderPassOutput> renderOutput)
{
	m_outputs.push_back(std::move(renderOutput));
}

void RenderPass::LinkOutput(const char* outputName, const std::string& linkedResource)
{
	RenderPassOutput* output = GetOutput(outputName);

	size_t positionOfDotThatSeparates = linkedResource.find('.');

	if (positionOfDotThatSeparates == std::string::npos)
	{
		std::string errorString = "Invalid string structure was passed as resource symbol. Passed string was: \"";
		errorString += linkedResource;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}

	if (positionOfDotThatSeparates == linkedResource.size() - 1)
	{
		std::string errorString = "Name of object wasn't specified in string. Passed string was: \"";
		errorString += linkedResource;
		errorString += "\".";

		THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
	}

	std::string passName = std::string(linkedResource.begin(), linkedResource.begin() + positionOfDotThatSeparates);
	std::string objectName = linkedResource.c_str() + positionOfDotThatSeparates + 1;

	output->Link(objectName, passName);
}

void RenderPass::CheckPassIntegrity() const
{
	for (const auto& input : m_inputs)
		input->CheckInputIntegrity();
	
	for (const auto& output : m_outputs)
		output->CheckOutputIntegrity();
}
