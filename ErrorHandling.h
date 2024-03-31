#pragma once
#include "Includes.h"
#include <exception>
#include "ErrorMacros.h"

#ifdef _DEBUG
#include <wrl.h>
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

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
		const char* GetLine();

	 private:
		std::string TranslateErrorCode(HRESULT hr);

	 protected:
		UINT32 m_line;
		const char* file;
		HRESULT m_hr;
	};

	class InternalException : public StandardException
	{
	public:
		InternalException(UINT32 line_, const char* file_, std::string errorString_);

	public:
		const char* what() override;
		virtual std::string GetErrorType() override;
		std::string GetErrorString() override;

	private:
		std::string m_errorString;
	};

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

	 class NoGFXException : public StandardException
	 {
	 public:
		 NoGFXException(UINT32 line_, const char* file_);

		 std::string GetErrorType() override;
		 HRESULT GetErrorCode() override;
	 };

	 class ModelException : public InternalException
	 {
	 public:
		 ModelException(UINT32 line_, const char* file_, std::string errorString_) : InternalException(line_, file_, errorString_) {};

	 public:
		 std::string GetErrorType() override;
	 };

#ifdef _DEBUG
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
	
	class RenderGraphException : public InternalException
	{
	public:
		RenderGraphException(UINT32 line_, const char* file_, std::string errorString_) : InternalException(line_, file_, errorString_) {};

	public:
		std::string GetErrorType() override;
	};

public:
	static VOID ThrowMessage(const char* title, const char* text) noexcept;
};

static ErrorHandler errorHandler;

#ifdef _DEBUG
	static ErrorHandler::DXGIException::DXGIInfoManager DXGImanager;
#endif