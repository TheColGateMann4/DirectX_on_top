#include "RenderPassOutput.h"
#include "RenderPassInput.h"
#include "ErrorMacros.h"

RenderPassOutput::RenderPassOutput(const char* name)
	: m_name(name)
{

}

void RenderPassOutput::Link(std::string& objectName, std::string& passName)
{
	{
		if (passName.empty())
			THROW_RENDER_GRAPH_EXCEPTION("passName was empty");

		for (char character : passName)
		{
			if (!isalpha(character) && character != '$')
			{
				std::string errorString = "Invalid Pass name was passed to output. Pass name was: \"";
				errorString += passName;
				errorString += "\".";

				THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
			}
		}
	}

	m_linkPassName = passName;

	{
		if (objectName.empty())
			THROW_RENDER_GRAPH_EXCEPTION("objectName was empty");

		for (char character : objectName)
		{
			if (!isalpha(character))
			{
				std::string errorString = "Invalid Object name was passed to output. Object name was: \"";
				errorString += objectName;
				errorString += "\".";

				THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
			}
		}
	}

	m_linkObjectName = objectName;
}

void RenderPassOutput::Bind(RenderPassInput& renderPassInput)
{
	std::string errorMessage = "Tried to bind invalid RenderPassOutput.";
	errorMessage += GetLocalInfo();

	THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());
}

const char* RenderPassOutput::GetOutputName() const
{
	return m_name;
}

std::string RenderPassOutput::GetLinkObjectName() const
{
	return m_linkObjectName;
}

std::string RenderPassOutput::GetLinkObjectOwnerName() const
{
	return m_linkPassName;
}

std::string RenderPassOutput::GetLocalInfo() const
{
	std::string result = " Ouput name was: \"";
	result += GetOutputName();
	result += "\"\nLinked object owner name was: \"";
	result += GetLinkObjectOwnerName();
	result += "\"\nLinked object name was: ";
	result += GetLinkObjectName();
	result += "\"\n";

	return result;
}