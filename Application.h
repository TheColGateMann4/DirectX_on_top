#pragma  once
#include "Includes.h"
#include "Window.h"
#include "ErrorHandling.h"
#include "Time.h"
#include "Shape.h"
#include "Cube.h"
#include "Sphere.h"

class Application
{
public:
	Application(UINT32 width, UINT32 height, const char* name);
	BOOL Initiate();

private:
	VOID DoFrame();

private:
	UINT32 m_width;
	UINT32 m_height;
	const char* m_name;

private:
	Window window;
	Time time;

private:
	std::vector<std::unique_ptr<Cube>> boxes;
};

