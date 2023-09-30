#pragma once

#undef UNICODE

//windows version
#define _WIN32_WINNIT = 0x0601
#include <sdkddkver.h>

//removing useless stuff from windows.h
// #define WIN32_LEAN_AND_MEAN
// #define NOGDICAPMASKS
// #define NOSYSMETRICS
// #define NOMENUS
// #define NOICONS
// #define NOSYSCOMMANDS
// #define NORASTEROPS
// #define OEMRESOURCE
// #define NOATOM
// #define NOCOLOR
// #define NOCTLMGR
// #define NODRAWTEXT
// #define NOKERNEL
// #define NONLS
// #define NOMEMMGR
// #define NOMETAFILE
// #define NOSCROLL
// #define NOSERVICE
// #define NOSOUND
// #define NOTEXTMETRIC
// #define NOWH
// #define NOCOMM
// #define NOKANJI
// #define NOHELP
// #define NOPROFILER
// #define NODEFERWINDOWPOS
// #define NOMCX
// #define NORPC
// #define NOPROXYSTUB
// #define NOIMAGE
// #define NOTAPE
// 
// #define STRICT
#include <windows.h>

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <vector>
#include <memory>
#include <random>

#define OBJLOADER_READ_SIZE 1024

#ifdef _DEBUG
	#include <iostream>
	#define IS_DEBUG TRUE
#else
	#define IS_DEBUG FALSE
#endif

//std::string os_;
//os_ += 'k';
//os_ += std::to_string(wParam);
//os_ += ' ';
//OutputDebugStringW(std::wstring(os_.begin(), os_.end()).c_str());

/*

		size_t len = strlen(except.what()) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), except.what(), len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();

*/

struct Vector2uint {
	UINT32 x, y;
};
 
struct Vector2int {
	INT32 x, y;
};

struct Vector2f {
	FLOAT x, y, z;
};

struct Vector3f {
	FLOAT x, y, z;
};

struct Vector4f {
	FLOAT x, y, z, t;
};

struct Colorc {
	unsigned char r, g, b;
};