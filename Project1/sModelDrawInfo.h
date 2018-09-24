#ifndef _sModelDrawInfo_HG_
#define _sModelDrawInfo_HG_
#include <string>
#include "sPlyVertex.h"
#include "sPlyTriangle.h"

static const struct sVert_xyz_rgb //This is based on what the shader is expecting 
{
	float x, y, z; //location
	float r, g, b; //color
};

struct sModelDrawInfo
{
	sModelDrawInfo();

	std::string meshFileName;
	unsigned int VAO_ID;

	unsigned int vertexBufferID;
	unsigned int vertexBuffer_start_index;
	unsigned int numberOfVertices;

	unsigned int indexBufferID;
	unsigned int indexBuffer_start_index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	//This is vertex informationas read from the file 
	sPlyVertex* pVerticesFromFile;  //

	//This is vertex informationas being passed to the GPU
	// (it is in format that shader needs)
	sVert_xyz_rgb* pVerticesToVBO;

	//This buffer is indices as a 1D array (that the GPU needs)
	unsigned int* pIndices;

	// These are the original triangles values from file,
	//but in the triangles or "face" format
	sPlyTriangle* pTriangles;  //=0

	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	float extentX, extentY, extentZ;
	float maxExtent;

};

#endif