#pragma once
#include "Includes.h"

namespace StringConverting
{
	std::string WideToNarrow(std::wstring str)
	{
		char buffer[MAX_PATH];
		wcstombs_s(nullptr, buffer, str.data(), MAX_PATH);
	}

	std::wstring NarrowToWide(std::string str)
	{
		wchar_t buffer[MAX_PATH];
		mbstowcs_s(nullptr, buffer, str.data(), MAX_PATH);
	}
}