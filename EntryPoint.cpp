#include "Application.h"
#include "ErrorHandling.h"

int WINAPI WinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	try
	{
		Application App(800, 600, "DirectX Dominating");
		return App.Initiate();
	}
	catch (ErrorHandler::StandardException& except)
	{
		errorHandler.ThrowMessage(NULL, "Internal Error", except.what());
	}
	catch ( std::exception& except)
	{
		errorHandler.ThrowMessage(NULL, "Standard Error", except.what());
	}
	catch (...)
	{			
		errorHandler.ThrowMessage(NULL, "Unknown Error", "No Details Available");
	}

	return FALSE;
}