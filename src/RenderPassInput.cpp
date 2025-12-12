#include "RenderPassInput.h"
#include "RenderPassOuput.h"
#include "ErrorMacros.h"

RenderPassInput::RenderPassInput(const char* name)
	: m_name(name)
{

}

void RenderPassInput::Link(std::string& objectName, std::string& passName)
{
	{
		if (passName.empty())
			THROW_RENDER_GRAPH_EXCEPTION("passName was empty");

		for (char character : passName)
		{
			if (!isalpha(character) && character != '$')
			{
				std::string errorString = "Invalid Pass name was passed to input. Pass name was: \"";
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
				std::string errorString = "Invalid Object name was passed to input. Object name was: \"";
				errorString += objectName;
				errorString += "\".";

				THROW_RENDER_GRAPH_EXCEPTION(errorString.c_str());
			}
		}
	}

	m_linkObjectName = objectName;
}

void RenderPassInput::Bind(RenderPassOutput& renderPassOutput)
{
	std::string errorMessage = "Tried to bind invalid RenderPassInput.";
	errorMessage += GetLocalInfo();

	THROW_RENDER_GRAPH_EXCEPTION(errorMessage.c_str());
}

const char* RenderPassInput::GetInputName() const
{
	return m_name;
}

std::string RenderPassInput::GetLinkObjectName() const
{
	return m_linkObjectName;
}

std::string RenderPassInput::GetLinkObjectOwnerName() const
{
	return m_linkPassName;
}

std::string RenderPassInput::GetLocalInfo() const
{
	std::string result = " Input name was: \"";
	result += GetInputName();
	result += "\"\nLinked object owner name was: \"";
	result += GetLinkObjectOwnerName();
	result += "\"\nLinked object name was: ";
	result += GetLinkObjectName();
	result += "\"\n";

	return result;
}