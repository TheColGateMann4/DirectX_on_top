#pragma once
#include <windows.h>
#include <type_traits>
#include <vector>
#include "ObjLoader.h"

#define VERTEXELEMENTNUM 0
#define INDICEELEMENTNUM 1
#define NORMALELEMENTNUM 2
#define TEXTUREELEMENTNUM 3

#define THESAME(class1, class2) std::is_same_v< class1, class2>


template<class V, class VN, class VTC>
template<class I>
inline VOID ObjLoader<V, VN, VTC>::ProcessData(std::string& data, bool CounterClockWise, size_t indexesPerNode)
{
	if constexpr (THESAME(I, V))
		m_verticies.emplace_back();
	if constexpr (THESAME(I, VN) && !THESAME(VN, void))
		m_normals.emplace_back();
	if constexpr (THESAME(I, VTC) && !THESAME(VTC, void))
		m_textureCoords.emplace_back();

	size_t currlength = 0;

	for (size_t currNumber = 0; currNumber < (THESAME(I, VTC) ? 2 : indexesPerNode); currNumber++) // only 3 numbers per line
	{
		size_t nextSpaceCharacter = data.find(' ', currlength);
		if (nextSpaceCharacter == std::string::npos)
			nextSpaceCharacter = data.size();

		std::string onlyNumberData = std::string(data.begin() + currlength, data.end() - (data.size() - nextSpaceCharacter));

		//gotta take care of the text after / or // first
		size_t positionOfDoubleSlash = onlyNumberData.find("//");
		size_t positionOfSlash = onlyNumberData.find("/");
		std::string AdditionalNumberData;
		{
			if (positionOfDoubleSlash != std::string::npos)
			{
				AdditionalNumberData = std::string(onlyNumberData.begin() + positionOfDoubleSlash + 2, onlyNumberData.end()); //not sure if +2 is correct
				onlyNumberData = std::string(onlyNumberData.begin(), onlyNumberData.end() - AdditionalNumberData.length() + 2);
			}
			else if (positionOfSlash != std::string::npos)
			{
				AdditionalNumberData = std::string(onlyNumberData.begin() + positionOfSlash + 1, onlyNumberData.end());
				onlyNumberData = std::string(onlyNumberData.begin(), onlyNumberData.end() - (AdditionalNumberData.length() + 1));
			}
		}
		//translating string to number
		if constexpr (THESAME(I, size_t))						//indices
			m_indicies.push_back(stoi(onlyNumberData) - 1);			// string to int
		else if constexpr (THESAME(I, V))							// vertices
			((FLOAT*)&m_verticies.back())[currNumber] = stof(onlyNumberData);		// string to float
		else if constexpr (THESAME(I, VN) && !THESAME(VN, void))			// normals
			((FLOAT*)&m_normals.back())[currNumber] = stof(onlyNumberData);
		else if constexpr (THESAME(I, VTC) && !THESAME(VTC, void))			// texture coords
			((FLOAT*)&m_textureCoords.back())[currNumber] = stof(onlyNumberData);
		else
			return; // we just ignore the input if its nothing we look for

		if constexpr (THESAME(I, size_t))
		{
			if constexpr (!THESAME(VN, void))
				if (positionOfDoubleSlash != std::string::npos)
					m_normals.at(stoi(AdditionalNumberData) - 1).index = m_indicies.back();
			if constexpr (!THESAME(VTC, void))
				if (positionOfSlash != std::string::npos)
					m_textureCoords.at(stoi(AdditionalNumberData) - 1).index = m_indicies.back();
		}

		currlength = nextSpaceCharacter + 1;
	}
	if constexpr (std::is_same_v<I, size_t>)
	{
		if (indexesPerNode == 4) // 4 vertices to 2 triangles
		{
			size_t vertorLen = m_indicies.size() - 1;
			std::swap(m_indicies[vertorLen - 1], m_indicies[vertorLen]);
			m_indicies.insert(m_indicies.end() - 1, m_indicies.at(vertorLen - 1));
			m_indicies.insert(m_indicies.end() - 1, m_indicies.at(vertorLen - 2));
		}
		/*if (CounterClockWise)
		{
			size_t indexLength = m_indicies.size();
			std::swap(m_indicies.at(indexLength - 1), m_indicies.at(indexLength - 2));
		}*/
	}
}
static std::string arrrrrrrrrrrrrrrrrrrrrrrr;
template<class V, class VN, class VTC>
inline ObjLoader<V, VN, VTC>::ObjLoader(std::wstring objpath, bool CounterClockWise)
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

	size_t totalFileSize = GetFileSize(hFile, nullptr);

	for (size_t totalBytesRead = 0; totalBytesRead < totalFileSize;)
	{
		std::string sbuffer(OBJLOADER_READ_SIZE, '\0');
		OVERLAPPED overlapped = {};
		overlapped.Offset = totalBytesRead;

		if (ReadFile(hFile, &sbuffer[0], OBJLOADER_READ_SIZE, nullptr, &overlapped) != 1)
			std::abort();

		size_t bytesRead = overlapped.InternalHigh;
		if (bytesRead == 0)
			std::abort();

		size_t elementEndingPos = 0;
		for (;;)
		{

			size_t elementStartingPos[4] = {};
			elementStartingPos[VERTEXELEMENTNUM] = sbuffer.find('v', elementEndingPos);
			elementStartingPos[INDICEELEMENTNUM] = sbuffer.find('f', elementEndingPos);
			elementStartingPos[NORMALELEMENTNUM] = sbuffer.find("vn", elementEndingPos);
			elementStartingPos[TEXTUREELEMENTNUM] = sbuffer.find("vt", elementEndingPos);

			if (this->CheckForNPOS(elementStartingPos))
				break;

			size_t smallestNumber = this->GetIndexOfSmallestNumber(elementStartingPos);
			totalBytesRead += elementStartingPos[smallestNumber] - elementEndingPos;

			elementStartingPos[smallestNumber] += 2; // we skip v character and space after it to get straight to the data

			if (smallestNumber == NORMALELEMENTNUM || smallestNumber == TEXTUREELEMENTNUM)
				elementStartingPos[smallestNumber]++; // since we want to skip space after vn or vt


			//elementEndingPos = sbuffer.find(13, elementStartingPos[smallestNumber]);
			elementEndingPos = sbuffer.find('\n', elementStartingPos[smallestNumber]);

			if (elementEndingPos == std::string::npos)
				if (totalBytesRead + elementStartingPos[smallestNumber] < totalFileSize && totalBytesRead > totalFileSize - OBJLOADER_READ_SIZE)
					elementEndingPos = bytesRead;
				else
					break;

			std::string temp = std::string(sbuffer.begin() + elementStartingPos[smallestNumber], sbuffer.end() - (bytesRead - elementEndingPos));

			if (smallestNumber == VERTEXELEMENTNUM)
			{
				ProcessData<V>(temp);
			}
			else if (smallestNumber == INDICEELEMENTNUM)
			{
				size_t indexesPerNode = GetIndexesPerNode(temp);
				ProcessData<size_t>(temp, CounterClockWise, indexesPerNode);
			}
			else if (smallestNumber == NORMALELEMENTNUM)
			{
				ProcessData<VN>(temp);
			}
			else if (smallestNumber == TEXTUREELEMENTNUM)
			{
				ProcessData<VTC>(temp);
			}

			totalBytesRead += (elementEndingPos - (elementStartingPos[smallestNumber] - 2));

			if (smallestNumber == NORMALELEMENTNUM || smallestNumber == TEXTUREELEMENTNUM)
				totalBytesRead++;
		}
	}
}
#undef OBJLOADER_READ_SIZE


template<class V, class VN, class VTC>
inline size_t ObjLoader<V, VN, VTC>::GetIndexOfSmallestNumber(size_t* numbers)
{
	size_t result = SIZE_MAX;
	size_t index;
	for (size_t i = 0; i <= TEXTUREELEMENTNUM; i++)
		if (numbers[i] <= result) // <= becuase vn and vt have v as well
		{
			result = numbers[i];
			index = i;
		}

	return index;
}

template<class V, class VN, class VTC>
inline size_t ObjLoader<V, VN, VTC>::GetIndexesPerNode(std::string& data)
{
	size_t result = 0;
	for (char character : data)
		if (character == ' ')
			result++;
	return result + 1;
}

template<class V, class VN, class VTC>
inline bool ObjLoader<V, VN, VTC>::CheckForNPOS(size_t* numbers)
{
	for (size_t i = 0; i <= TEXTUREELEMENTNUM; i++)
		if (numbers[i] != std::string::npos)
			return false;
	return true;
}