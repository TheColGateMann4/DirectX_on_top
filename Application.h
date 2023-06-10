#pragma  once
#include "Includes.h"
#include "Window.h"
#include "ErrorHandling.h"
#include "Time.h"
#include "Shape.h"
#include "Sheet.h"

class Application
{
public:
	Application(UINT32 width, UINT32 height, const char* name);
	BOOL Initiate();

public:
	Window window;
	Time timer;

private:
	VOID DoFrame();

private:
	UINT32 m_width;
	UINT32 m_height;
	const char* m_name;

private:
	std::vector<std::unique_ptr<Shape>> boxes;
};

