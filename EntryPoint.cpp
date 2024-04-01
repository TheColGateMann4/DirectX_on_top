#include "Application.h"
#include "ErrorHandling.h"

int WINAPI WinMain
(
	_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ LPSTR,
	_In_ int
)
{
#ifdef IS_DEBUG
	AllocConsole();
	FILE* pConsole;
	freopen_s(&pConsole, "CONOUT$", "w", stdout);
#endif

	try
	{
		UINT32 screenWidth = UINT32(std::round(float(GetSystemMetrics(SM_CXSCREEN)) * 0.625f));
		UINT32 screenHeight = UINT32(std::round(float(GetSystemMetrics(SM_CYSCREEN)) * 0.83333333333f));

		Application App(screenWidth, screenHeight, "DirectX Dominating");

		return App.Initiate();
	}
	catch (ErrorHandler::StandardException& except)
	{
		errorHandler.ThrowMessage("Internal Error", except.what());
	}
	catch ( std::exception& except)
	{
		errorHandler.ThrowMessage("Standard Error", except.what());
	}
	catch (...)
	{			
		errorHandler.ThrowMessage("Unknown Error", "No Details Available");
	}

#ifdef IS_DEBUG
	fclose(pConsole);
	FreeConsole();
#endif

	return FALSE;
}