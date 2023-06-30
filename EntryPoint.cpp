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
		Application App(800, 600, "DirectX Dominating");
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

//TODO: Get Includes right
//TODO: Add ConstBuffer with time to vertex shader 