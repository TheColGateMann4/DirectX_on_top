#pragma once
#include "Includes.h"
/*
	   T needs to have member value .position and it needs to hold 3 float values
*/

template<class T>
class ObjLoader
{
public:
	ObjLoader<T>(std::wstring &objpath, bool CounterClockWise);

public:
	std::vector<T> GetVerticies();

	std::vector<UINT32> GetIndicies();

private:
	template<class C>
	C ProcessData(std::string &data, bool CounterClockWise = false);

private:
	std::vector<T> m_verticies;
	std::vector<UINT32> m_indicies;
};

#include "ObjLoader.inl"