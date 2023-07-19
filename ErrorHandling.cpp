#include "ErrorHandling.h"
#include "DXError.h"
#include <sstream>

ErrorHandler::StandardException::StandardException(UINT32 line_, const char* file_, HRESULT hr_)
	: m_line(line_), file(file_), m_hr(hr_)
{}

std::string ErrorHandler::StandardException::TranslateErrorCode(HRESULT hr)
{
	char* msgBuf = NULL;
	DWORD msgLen = FormatMessageA
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		reinterpret_cast<LPSTR>(&msgBuf),
		NULL,
		NULL
	);

	if (msgLen == 0)
		return "Undefined error code";

	return msgBuf;
}

std::string ErrorHandler::StandardException::GetErrorString()
{
	return TranslateErrorCode(m_hr);
}

HRESULT ErrorHandler::StandardException::GetErrorCode()
{
	return m_hr;
}

const char* ErrorHandler::StandardException::GetFile()
{
	return file;
}

const char* ErrorHandler::StandardException::GetLine()
{
	return _strdup(std::to_string(m_line).c_str());
}

std::string ErrorHandler::StandardException::GetErrorType()
{
	return "STANDARD_EXCEPTION";
}

const char* ErrorHandler::StandardException::what()
{
	std::string result;

	result += GetErrorType();

	result += "\n[Error Code]: ";
	result += std::to_string(GetErrorCode());

	result += "\n[Error String]: ";
	result += GetErrorString();

	result += "\n";

	result += "\n[File] ";
	result += GetFile();
	result += "\n[Line] ";
	result += GetLine();

	return _strdup(result.c_str());
}


















ErrorHandler::GFXException::GFXException(UINT32 line, const char* file, HRESULT hr)
	: StandardException(line, file, hr)
{}

HRESULT ErrorHandler::GFXException::GetErrorCode()
{
	return m_hr;
}

std::string ErrorHandler::GFXException::GetErrorString()
{
	return DXGetErrorString(m_hr);
}

std::string ErrorHandler::GFXException::GetErrorType()
{
	return "GFX_RESULT_EXCEPTION";
}


std::string ErrorHandler::GFXException::GetErrorDescription()
{
	char buff[350];
	DXGetErrorDescriptionA(m_hr, buff, sizeof(buff));
	return (std::string)buff;
}

const char* ErrorHandler::GFXException::what()
{
	std::stringstream result;

	result << GetErrorType();

	result << "\n[Error Code] ";
	result << GetErrorCode();

	result << "\n\n[Error Name] ";
	result << GetErrorString();

	result << "\n\n[Description] ";
	result << GetErrorDescription();

	result << "\n";

	result << "\n\n[File] ";
	result << GetFile();

	result << "\n\n[Line] ";
	result << GetLine();

	return _strdup(result.str().c_str());
}




ErrorHandler::InternalException::InternalException(UINT32 line_, const char* file_, std::string errorString_)
	: StandardException(line_, file_, NULL), m_errorString(errorString_)
{

}

std::string ErrorHandler::InternalException::GetErrorType()
{
	return "INTERNAL_EXCEPTION";
}
std::string ErrorHandler::InternalException::GetErrorString()
{
	return m_errorString;
}

const char* ErrorHandler::InternalException::what()
{
	std::stringstream result;

	result << GetErrorType();

	result << "\n[Error Name] ";
	result << GetErrorString();

	result << "\n\n[File] ";
	result << GetFile();

	result << "\n[Line] ";
	result << GetLine();

	return _strdup(result.str().c_str());
}







ErrorHandler::NoGFXException::NoGFXException(UINT32 line_, const char* file_)
	: StandardException(line_, file_, NULL)
{

}

std::string ErrorHandler::NoGFXException::GetErrorType()
{
	return "NO_GFX_EXCEPTION";
}

HRESULT ErrorHandler::NoGFXException::GetErrorCode()
{
	return (HRESULT)0;
}



std::string ErrorHandler::ModelException::GetErrorType()
{
	return "MODEL_EXCEPTION";
}








VOID ErrorHandler::ThrowMessage(const char* title, const char* text) noexcept
{
	MessageBoxA(NULL, text, title, MB_OK | MB_ICONEXCLAMATION);

	return;
}



















#ifdef _DEBUG

ErrorHandler::DXGIException::DXGIException(UINT32 line_, const char* file_, HRESULT hr_, std::vector<std::string> messages)
	: GFXException(line_, file_, hr_)
{

	for (std::string message : messages)
		allMessages += (message + '\n');

	if (!allMessages.empty()) //removing '\n' at the end
		allMessages.pop_back();
}

std::string ErrorHandler::DXGIException::GetErrorInfo()
{
	return allMessages;
}

std::string ErrorHandler::DXGIException::GetErrorType()
{
	return "DIRECTX_EXCEPTION";
}

const char* ErrorHandler::DXGIException::what()
{
	std::stringstream result;

	result << GetErrorType();

	result << "\n[Error Code] ";
	result << GetErrorCode();

	result << "\n[Error Name] ";
	result << GetErrorString();

	result << "\n\n[Description] ";
	result << GetErrorDescription();

	result << "\n\n[Error Info] ";
	result << GetErrorInfo();

	result << "\n\n[File] ";
	result << GetFile();

	result << "\n[Line] ";
	result << GetLine();

	return _strdup(result.str().c_str());
}
#define THROW_NOINFO(checkfailed) if( FAILED( hr = (checkfailed) ) ) throw GFXException( __LINE__,__FILE__,hr );

ErrorHandler::DXGIException::DXGIInfoManager::DXGIInfoManager()
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
		THROW_LAST_ERROR;

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
		THROW_LAST_ERROR;

	HRESULT hr;
	THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

void ErrorHandler::DXGIException::DXGIInfoManager::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> ErrorHandler::DXGIException::DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength = 0;

		// get the size of message i in bytes
		THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// get the message and push its description into the vector
		THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}

std::string ErrorHandler::DeviceRemovedException::GetErrorType()
{
	return "DEVICE_REMOVED_EXCEPTION";
}





ErrorHandler::InfoException::InfoException(UINT32 line_, const char* file_, std::vector<std::string> messages)
	: GFXException(line_, file_, NULL)
{
	for (std::string message : messages)
		ErrorHandler::InfoException::allMessages += (message + '\n');

	//removing '\n' at the end
	if (!allMessages.empty())
		allMessages.pop_back();
}

std::string ErrorHandler::InfoException::GetErrorType()
{
	return "DIRECTX_INFO_EXCEPTION";
}

std::string ErrorHandler::InfoException::GetErrorInfo()
{
	return allMessages;
}

const char* ErrorHandler::InfoException::what()
{
	std::stringstream result;

	result << GetErrorType();

	result << "\n\n[Error Info] ";
	result << GetErrorInfo();

	result << "\n\n[File] ";
	result << GetFile();

	result << "\n[Line] ";
	result << GetLine();

	return _strdup(result.str().c_str());
}

#endif