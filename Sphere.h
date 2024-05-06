#pragma once
#include "SimpleMesh.h"

class Sphere
{

public:
	static SimpleMesh GetMesh(float radius = 1.0f, UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24);

	// Sphere generating algorithm taken from StackOverflow, made by Rabbid76
	//https://stackoverflow.com/questions/45482988/generating-spheres-vertices-indices-and-normals
	static SimpleMesh GetMeshWithNormalsAndTextureMapping(float radius = 1.0f, UINT32 verticalDivisions = 12, UINT32 horizontalDivisions = 24);

	// Tangent Calculation Logic is taken from Assimp model parser. File: CalcTangentsProcess.cpp ProcessMesh()
	// https://github.com/assimp/assimp/blob/master/code/PostProcessing/CalcTangentsProcess.cpp
	static void CalculateTangents(SimpleMesh& mesh);
};

