#include "sModelDrawInfo.h"

sModelDrawInfo::sModelDrawInfo() {

	std::string meshFileNama;
	this -> VAO_ID = 0;

	this-> vertexBufferID = 0;
	this-> vertexBuffer_start_index = 0;
	this-> numberOfVertices;

	this-> indexBufferID = 0;
	this-> indexBuffer_start_index = 0;
	this-> numberOfIndices = 0;
	this-> numberOfTriangles = 0;

	this->pVerticesFromFile = NULL;
	this->pVerticesToVBO = NULL;
	this-> pIndices = NULL;
	this-> pTriangles = NULL; 

	this-> maxX = this-> maxY = this-> maxZ = 0.0f;
	this-> minX = this-> minY = this-> minZ = 0.0f;

	this-> extentX = this-> extentY = this-> extentZ = 0.0f;
	this->maxExtent = 0.0f;

	return;
}