#pragma once
#include "Includes.h"
#include "ErrorMacros.h"
#include <exception>

#ifdef _DEBUG
#include <wrl.h>
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

//#define THROW_GFX_DEVICE_REMOVED(hr) int a

class ErrorHandler
{
 public:
	class StandardException : public std::exception
	{
	 public:
		StandardException(UINT32 line_, const char* file_, HRESULT hr_);

	 public:
		virtual const char* what();

		virtual std::string GetErrorType();
		virtual std::string GetErrorString();
		virtual HRESULT GetErrorCode();

		const char* GetFile();
		UINT32 GetLine();

	 private:
		std::string TranslateErrorCode(HRESULT hr);

	 protected:
		UINT32 line;
		const char* file;
		HRESULT hr;
	};

	class InternalException : public StandardException
	{
	public:
		InternalException(UINT32 line_, const char* file_, std::string errorString_);

	public:
		const char* what() override;
		std::string GetErrorType() override;
		 std::string GetErrorString() override;

	private:
		std::string m_errorString;
	};

 public:
	 class GFXException : public StandardException
	{
	 public:
		GFXException(UINT32 line_, const char* file_, HRESULT hr_);

	 public:
		const char* what() override;

		virtual std::string GetErrorType() override;
		virtual HRESULT GetErrorCode() override;
		virtual std::string GetErrorString() override;
		std::string GetErrorDescription();

	};
#ifdef _DEBUG
public:
	class DXGIException : public GFXException
	{
	 public:
		DXGIException(UINT32 line_, const char* file_, HRESULT hr_, std::vector<std::string> messages = {});

	 public:
		 const char* what() override;

		 std::string GetErrorType() override;
		 std::string GetErrorInfo();

	 protected:
		std::string allMessages;

	 public:
		class DXGIInfoManager
		{
		 public:
			DXGIInfoManager();

			void Set() noexcept;
			std::vector<std::string> GetMessages() const;

		 private:
			unsigned long long next = 0u;
			Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;

		};
	};
	class DeviceRemovedException : public GFXException
	{
		std::string GetErrorType() override;
	};
public:
	class NoGFXException : public StandardException
	{
	 public:
		NoGFXException(UINT32 line_, const char* file_);

		std::string GetErrorType() override;
		HRESULT GetErrorCode() override;
	};
	class InfoException : public GFXException
	{
	 public:
		InfoException(UINT32 line_, const char* file_, std::vector<std::string> messages = {});

	public:
		const char* what();

		std::string GetErrorType() override;
		std::string GetErrorInfo();

	private:
		std::string allMessages;
	};
#endif
	
 public:
	static VOID ThrowMessage(HWND hWnd, const char* title, const char* text) noexcept;
};

static ErrorHandler errorHandler;

#ifdef _DEBUG
	static ErrorHandler::DXGIException::DXGIInfoManager DXGImanager;
#endif