#pragma once

#include "ObjLoader.h"
#include <type_traits>

template<class T>
template<class C>
inline C ObjLoader<T>::ProcessData(std::string &data, bool CounterClockWise)
{
	C result;
	UINT32 currlength = 0;

	for (UINT32 currNumber = 0; currNumber <= 2; currNumber++) // only 3 numbers per line
	{
		size_t nextSpaceCharacter = data.find(' ', currlength);
		if (nextSpaceCharacter == std::string::npos)
			nextSpaceCharacter = data.size();

		std::string onlyNumberData = std::string(data.begin() + currlength, data.end() - (data.size() - nextSpaceCharacter));

		//translating string to number
		if constexpr (std::is_same_v<C, std::vector<UINT32>>) // indices
			result.push_back(stoi(onlyNumberData) - 1); // string to int
		else if constexpr (std::is_same_v<C, T>) // vertices
			((FLOAT*)&result)[currNumber] = stof(onlyNumberData); // string to float
		else
			std::abort();

		currlength = nextSpaceCharacter + 1;
	}
	if (CounterClockWise)
		if constexpr (std::is_same_v<C, std::vector<UINT32>>)
			std::swap(result[1], result[2]);
	return result;
}

template<class T>
inline ObjLoader<T>::ObjLoader(std::wstring &objpath, bool CounterClockWise)
{
	HANDLE hFile = CreateFileW(
		objpath.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	UINT32 totalFileSize = GetFileSize(hFile, nullptr);

	for (UINT32 currBytesRead = 0; currBytesRead < totalFileSize;)
	{
		std::string sbuffer(OBJLOADER_READ_SIZE + 1, '\0');
		OVERLAPPED overlapped = {};
		overlapped.Offset = currBytesRead;

		if (ReadFile(hFile, &sbuffer[0], OBJLOADER_READ_SIZE, nullptr, &overlapped) != 1)
			std::abort();

		UINT32 bytesRead = overlapped.InternalHigh;

		if (bytesRead == 0)
			std::abort();

		size_t elementEndingPos = 0;
		for (;;)
		{
			size_t vertexElementStartingPos = sbuffer.find('v', elementEndingPos);
			size_t indiceElementStartingPos = sbuffer.find('f', elementEndingPos);
			if (indiceElementStartingPos == std::string::npos && vertexElementStartingPos == std::string::npos)
				break;

			if (vertexElementStartingPos < indiceElementStartingPos)
			{
				UINT32 elementStartingPos = vertexElementStartingPos + 2; // we skip v character and space after it to get straight to the data
				UINT32 numberOfBytesThatWillBeReadInTotalAfterThisTime = bytesRead - elementStartingPos + currBytesRead;
				elementEndingPos = sbuffer.find(13, elementStartingPos);
				if (elementEndingPos == std::string::npos)
					if (numberOfBytesThatWillBeReadInTotalAfterThisTime != totalFileSize)
						break;
					else
						elementEndingPos = sbuffer.size();

				std::string temp = std::string(sbuffer.begin() + elementStartingPos, sbuffer.end() - (sbuffer.size() - elementEndingPos));
				m_verticies.push_back(ProcessData<T>(temp));

				currBytesRead += ((elementEndingPos + 2) - (elementStartingPos - 2));
			}
			else
			{
				UINT32 elementStartingPos = indiceElementStartingPos + 2; // we skip f character and space after it to get straight to the data
				UINT32 numberOfBytesThatWillBeReadInTotalAfterThisTime = bytesRead - (elementStartingPos - 2) + currBytesRead;
				elementEndingPos = sbuffer.find(13, elementStartingPos);
				if (elementEndingPos == std::string::npos)
					if (numberOfBytesThatWillBeReadInTotalAfterThisTime != totalFileSize)
						break;
					else
						elementEndingPos = sbuffer.size();

				UINT32 lengthOfString = (sbuffer.find(13, elementStartingPos) - elementStartingPos);
				std::string temp = std::string(sbuffer.begin() + elementStartingPos, sbuffer.end() - (sbuffer.size() - elementEndingPos));
				std::vector<UINT32> processedData = ProcessData<std::vector<UINT32>>(temp, CounterClockWise);
				m_indicies.insert(m_indicies.end(), processedData.begin(), processedData.end());

				currBytesRead += ((elementEndingPos + 2) - (elementStartingPos - 2));
			}
		}
	}
}

template <class T>
inline std::vector<T> ObjLoader<T>::GetVerticies()
{
	return m_verticies;
}

template <class T>
inline std::vector<UINT32> ObjLoader<T>::GetIndicies()
{
	return m_indicies;
}