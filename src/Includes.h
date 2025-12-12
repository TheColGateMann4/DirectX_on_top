#pragma once

#undef UNICODE

//windows version
#define _WIN32_WINNIT = 0x0601
#include <sdkddkver.h>

 #define NOGDICAPMASKS
 #define NOMENUS
 #define NOICONS
 #define NOSYSCOMMANDS
 #define NORASTEROPS
 #define OEMRESOURCE
 #define NOATOM
 #define NOCOLOR
 #define NODRAWTEXT
 #define NOKERNEL
 #define NOMEMMGR
 #define NOMETAFILE
 #define NOSCROLL
 #define NOSERVICE
 #define NOSOUND
 #define NOTEXTMETRIC
 #define NOWH
 #define NOCOMM
 #define NOKANJI
 #define NOHELP
 #define NOPROFILER
 #define NODEFERWINDOWPOS
 #define NOMCX
 #define NORPC
 #define NOPROXYSTUB
 #define NOIMAGE
 #define NOTAPE
 
#include <windows.h>

#include <d3d11.h>
#include <DirectXMath.h>

#include <string>
#include <vector>
#include <memory>
#include <random>


constexpr float _Pi = 3.14159265358979f;

#ifdef _DEBUG
	#include <iostream>
	#define IS_DEBUG TRUE
#else
	#define IS_DEBUG FALSE
#endif

#pragma warning(disable:4100)