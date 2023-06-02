#pragma once
#include "ErrorHandling.h"

#define THROW_ERROR(hr_) throw ErrorHandler::StandardException{ (UINT32)__LINE__, __FILE__, hr_ }; 
#define THROW_LAST_ERROR throw ErrorHandler::StandardException{ (UINT32)__LINE__, __FILE__, HRESULT_FROM_WIN32(GetLastError()) }; 

#define THROW_GFX_FAILED(hr) throw ErrorHandler::GFXException{ (UINT32)__LINE__, __FILE__, hr, };
#define THROW_NOGFX throw ErrorHandler::NoGFXException{(UINT32)__LINE__, __FILE__};

#define THROW_INTERNAL_ERROR(errorString, statement) if(statement)throw ErrorHandler::InternalException{ (UINT32)__LINE__ , __FILE__ , errorString};

#ifdef _DEBUG
	#define THROW_GFX_IF_FAILED(checkfailed) DXGImanager.Set(); if( FAILED( hr = (checkfailed) ) ){throw ErrorHandler::DXGIException{ (UINT32)__LINE__, __FILE__, hr, DXGImanager.GetMessages()}; }
	#define THROW_GFX_DEVICE_REMOVED(hr) throw ErrorHandler::DXGIException{(UINT32)__LINE__, __FILE__, hr, DXGImanager.GetMessages() };
	#define THROW_INFO_EXCEPTION(info) {DXGImanager.Set(); (info); std::vector<std::string>a = DXGImanager.GetMessages(); if(!a.empty()) throw ErrorHandler::InfoException{(UINT32)__LINE__, __FILE__, a};}
#else
	#define THROW_GFX_IF_FAILED(checkfailed) if(FAILED(hr = (checkfailed))) throw ErrorHandler::GFXException{(UINT32)__LINE__, __FILE__, hr};
	#define THROW_GFX_DEVICE_REMOVED(hr) throw ErrorHandler::GFXException{(UINT32)__LINE__, __FILE__, hr};
	#define THROW_INFO_EXCEPTION(info) (info);
#endif