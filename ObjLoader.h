#pragma once
#include "Includes.h"

#define OBJLOADER_READ_SIZE 1024
#define ARG_CHECK(argcheck) !std::is_same_v<argcheck, void>
#define TYPE_WITH_ARGV_CHECK(membertype) std::conditional<ARG_CHECK(membertype), std::vector<membertype>, int>::type

template<class V, class VN = void, class VTC = void>
class ObjLoader
{
public:
	ObjLoader<V, VN, VTC>(std::wstring objpath, bool CounterClockWise = false);

public:
	std::vector<UINT32> GetIndicies() { return this->m_indicies; };
	std::vector<V> GetVerticies() { return this->m_verticies; };
	std::vector<VN> GetNormals() requires (ARG_CHECK(VN)) { return this->m_normals; };
	std::vector<VTC> GetTextureCoords() requires (ARG_CHECK(VTC)) { return this->m_textureCoords; };

private:
	template<class I>
	VOID ProcessData(std::string& data, bool CounterClockWise = false, size_t indexesPerNode = 3);


private:
	size_t GetIndexOfSmallestNumber(size_t* numbers);
	size_t GetIndexesPerNode(std::string& data);
	bool CheckForNPOS(size_t* numbers);

private:
	std::vector<UINT32> m_indicies;
	std::vector<V> m_verticies;
	TYPE_WITH_ARGV_CHECK(VN) m_normals;
	TYPE_WITH_ARGV_CHECK(VTC) m_textureCoords;
};

#include "ObjLoader.inl"
#undef ARG_CHECK
#undef TYPE_WITH_ARGV_CHECK