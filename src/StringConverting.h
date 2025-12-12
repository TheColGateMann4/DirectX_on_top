#pragma once
#include "Includes.h"

namespace StringConverting
{
	std::string WideToNarrow(std::wstring str)
	{
		char buffer[MAX_PATH];
		wcstombs_s(nullptr, buffer, str.data(), MAX_PATH);
		return std::string(std::move(buffer));
	}

	std::wstring NarrowToWide(std::string str)
	{
		wchar_t buffer[MAX_PATH];
		mbstowcs_s(nullptr, buffer, str.data(), MAX_PATH);
		return std::wstring(std::move(buffer));
	}
}