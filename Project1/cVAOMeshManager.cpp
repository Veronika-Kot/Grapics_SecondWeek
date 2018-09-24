#include "cVAOMeshManager.h"
#include "globalStuff.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


//static const struct sVert_xyz_rgb //This is based on what the shader is expecting 
//{
//	float x, y, z; //location
//	float r, g, b; //color
//};

static const struct sVert_xyz_rgb;

cVAOMeshManager::cVAOMeshManager() {
	
}

cVAOMeshManager::~cVAOMeshManager() {
	
}

bool cVAOMeshManager::LoadModelIntoVAO(sModelDrawInfo &drawInfo,
	unsigned int shaderProgramID) {

	//Check if the model was loaded
	std::map< std::string /*model name*/, sModelDrawInfo >::iterator itModel = this->m_mapModel_to_VAOID.find( drawInfo.meshFileName );

	if ( itModel != this->m_mapModel_to_VAOID.end() ) {
		this->m_AppendTextToLastError("Model", false);
		this->m_AppendTextToLastError(drawInfo.meshFileName, false);
		this->m_AppendTextToLastError(" Was loaded already");
		return false;
	}

	//Load the model from the file
	if (!this->m_LoadModelFromFile(drawInfo)) {
		std::cout << "Can't load model from the file" << std::endl;
		return false;
	}

	//Load into GPU
	if ( ! this->m_LoadDrawInfo_Into_VAO(drawInfo, shaderProgramID)) {
		std::cout << "Can't upload model to the VAO" << std::endl;
		return false;
	}

	//Store this mesh into the map
	this->m_mapModel_to_VAOID[drawInfo.meshFileName] = drawInfo;

	return true;
}


bool cVAOMeshManager::FindDrawInfoByModelName(sModelDrawInfo &drawInfo) {

	std::map< std::string /*model name*/, sModelDrawInfo >::iterator itModel = this->m_mapModel_to_VAOID.find(drawInfo.meshFileName);
	
	if (itModel == this->m_mapModel_to_VAOID.end()) {
		std::cout << "Din't find a file name in the map of meshes"<<std::endl;
		return false;
	}

	//copy the draw info back to the caller
	drawInfo = itModel->second;
	
	return true;
}

void cVAOMeshManager::ShutDown(void) {
	//TODO clean the map

	return;
}


bool cVAOMeshManager::m_LoadModelFromFile(sModelDrawInfo &drawInfo) {

	//***Open the file
	std::ifstream theFile( drawInfo.meshFileName.c_str()); //converting string to C-style string

	if (!theFile.is_open()) { //more c or c++ ish
		//std::cout << "Didn't open file" << std::endl;
		this->m_AppendTextToLastError("Didn't open the", false);
		this->m_AppendTextToLastError(drawInfo.meshFileName, false);
		this->m_AppendTextToLastError(" file.");
		
		return false;
	}


	//***search for vertex
	std::string nextData;
	while (theFile >> nextData) {
		if (nextData == "vertex") {
			break;
		}
	}

	//theFile >> g_numberOfVertices;
	theFile >> drawInfo.numberOfVertices;
	std::cout << "Vertices: " << drawInfo.numberOfVertices << std::endl;

	//search for face
	while (theFile >> nextData) {
		if (nextData == "face") {
			break;
		}
	}

	//theFile >> g_numberOfTriangles;
	theFile >> drawInfo.numberOfTriangles;
	std::cout << "Next Data" << nextData << std::endl;
	std::cout << "Triangles: " << drawInfo.numberOfTriangles << std::endl;

	//***search for end_header
	while (theFile >> nextData) {
		if (nextData == "end_header") {
			break;
		}
	}

	//***Create an array to store the data
	drawInfo.pVerticesFromFile = new sPlyVertex[drawInfo.numberOfVertices]; 
	memset(drawInfo.pVerticesFromFile, 0, sizeof(drawInfo.pVerticesFromFile) * drawInfo.numberOfVertices); //set the memory 
	//distanaition, value, how many bytes you want to overwrite (how many bytes * how many verts)


	//***read the vertex data into the array
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++) {
		theFile >> drawInfo.pVerticesFromFile[index].x >> drawInfo.pVerticesFromFile[index].y >> drawInfo.pVerticesFromFile[index].z;
	}

	//***Same with triangles

	
	drawInfo.pTriangles = new sPlyTriangle[drawInfo.numberOfTriangles];

	int gardbige = 0;
	for (unsigned int index = 0; index != drawInfo.numberOfTriangles; index++) {
		theFile >> gardbige;
		theFile >> drawInfo.pTriangles[index].vertex_index_1; //can be seperate
		theFile >> drawInfo.pTriangles[index].vertex_index_2;
		theFile >> drawInfo.pTriangles[index].vertex_index_3;
	}

	std::cout << "Read from the file ok" << std::endl;

	//****Calculating extens (min/max edges)
	//if we want to fit a model into a box 1x1x1
	//we need to do scale = 1.0f / MaximumExtent
	//2x2x2 --> 2.0f / MaximumExtent


	drawInfo.minX = drawInfo.pVerticesFromFile[0].x;
	drawInfo.minY = drawInfo.pVerticesFromFile[0].y;
	drawInfo.minZ = drawInfo.pVerticesFromFile[0].z;

	drawInfo.maxX = drawInfo.pVerticesFromFile[0].x;
	drawInfo.maxY = drawInfo.pVerticesFromFile[0].y;
	drawInfo.maxZ = drawInfo.pVerticesFromFile[0].z;

	for (unsigned int index = 1; index != drawInfo.numberOfVertices; index++) {
		if ( drawInfo.pVerticesFromFile[index].x < drawInfo.minX ) { drawInfo.minX = drawInfo.pVerticesFromFile[index].x; }
		if ( drawInfo.pVerticesFromFile[index].y < drawInfo.minY ) { drawInfo.minY = drawInfo.pVerticesFromFile[index].y; }
		if ( drawInfo.pVerticesFromFile[index].z < drawInfo.minZ ) { drawInfo.minZ = drawInfo.pVerticesFromFile[index].z; }

		if (drawInfo.pVerticesFromFile[index].x > drawInfo.maxX) { drawInfo.maxX = drawInfo.pVerticesFromFile[index].x; }
		if (drawInfo.pVerticesFromFile[index].y > drawInfo.maxY) { drawInfo.maxY = drawInfo.pVerticesFromFile[index].y; }
		if (drawInfo.pVerticesFromFile[index].z > drawInfo.maxZ) { drawInfo.maxZ = drawInfo.pVerticesFromFile[index].z; }
	}

	drawInfo.extentX = drawInfo.maxX - drawInfo.maxX;
	drawInfo.extentY = drawInfo.maxY - drawInfo.maxY;
	drawInfo.extentZ = drawInfo.maxZ - drawInfo.maxZ;

	drawInfo.maxExtent = drawInfo.extentX;
	if (drawInfo.maxExtent < drawInfo.extentY) { drawInfo.maxExtent = drawInfo.extentY; }
	if (drawInfo.maxExtent < drawInfo.extentZ) { drawInfo.maxExtent = drawInfo.extentZ; }

	return true;
}


bool cVAOMeshManager::m_LoadDrawInfo_Into_VAO(sModelDrawInfo &drawInfo,
		unsigned int shaderProgramID) {

	//***Alocate another array temp for the vertex buffer

	drawInfo.pVerticesToVBO = new sVert_xyz_rgb[drawInfo.numberOfVertices];

	//***Copy everything from PLY format to the vertex buffer format
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++) {

		//copy data to shader
		drawInfo.pVerticesToVBO[index].x = drawInfo.pVerticesFromFile[index].x;
		drawInfo.pVerticesToVBO[index].y = drawInfo.pVerticesFromFile[index].y;
		drawInfo.pVerticesToVBO[index].z = drawInfo.pVerticesFromFile[index].z;

		drawInfo.pVerticesToVBO[index].r = 1.0f;
		drawInfo.pVerticesToVBO[index].g = 1.0f;
		drawInfo.pVerticesToVBO[index].b = 1.0f;
	}

	//***Create a VAO (vertex Array object) Everything which ties to vertex array is remebered 
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	glBindVertexArray(drawInfo.VAO_ID);

	//***Alocate buffer
	GLuint  vpos_location, vcol_location;
	drawInfo.vertexBufferID;

	//Creates VBO = vertes buffer object
	glGenBuffers(1, &(drawInfo.vertexBufferID) ); //1 - is how many buffers do you want & - passing by refference, gives the address
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.vertexBufferID); //gives to a new buffer a type permanetly(never change)- "vertex buffer"

	unsigned int vertexBufferSizeInBytes = sizeof(sVert_xyz_rgb) * drawInfo.numberOfVertices; //we can do it bc it's local = in the stuck, so we can use pointer

	//It takes array of pVertices and copy it to GPU
	glBufferData(GL_ARRAY_BUFFER, //***MAKE ME A BUFFER
		vertexBufferSizeInBytes, //sizeof(vertices), //ALLOCATE THAT MANY BYTES
		drawInfo.pVerticesToVBO, //vertices, //FROM COPY DATA THIS ARRAY
		GL_STATIC_DRAW); //DRAW

	drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;
	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];
	memset(drawInfo.pIndices, 0, sizeof(unsigned int) * drawInfo.numberOfIndices);


	//***Create an array for index buffer
	unsigned int indexIndex = 0; //Index into the VERTEX array

	//usigned int 32bit
	for (unsigned int triIndex = 0;
		triIndex != drawInfo.numberOfTriangles;
		triIndex++, indexIndex += 3) {

		drawInfo.pIndices[indexIndex + 0] = drawInfo.pTriangles[triIndex].vertex_index_1;
		drawInfo.pIndices[indexIndex + 1] = drawInfo.pTriangles[triIndex].vertex_index_2;
		drawInfo.pIndices[indexIndex + 2] = drawInfo.pTriangles[triIndex].vertex_index_3;

	}

	//***Create a VAO (vertex Array object) Everything which ties to vertex array is remebered 
	glGenBuffers(1, &(drawInfo.indexBufferID)); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.indexBufferID); 

	unsigned int indexBufferSizeInBytes = sizeof(unsigned int) * drawInfo.numberOfIndices;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, //***MAKE ME A BUFFER
		indexBufferSizeInBytes, //sizeof(vertices), //ALLOCATE THAT MANY BYTES
		drawInfo.pIndices, //vertices, //FROM COPY DATA THIS ARRAY
		GL_STATIC_DRAW); //DRAW




	vpos_location = glGetAttribLocation(shaderProgramID, "vPos");
	vcol_location = glGetAttribLocation(shaderProgramID, "vCol");

	//Passing data here array above in format from vertex01.glsl
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, //"vPosition"
		3, // vec3 (xyz) 
		GL_FLOAT,
		GL_FALSE, // Don't normilize
		sizeof(float) * 6, //after you find the first value vertex, where to 
						   // find the next vertex -> 6 floats away, 6 for xyz+rgb
		(void*)0); //start at 0

	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, //cColor
		3,  //vec3 rgb
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 6, //changed from 5
		(void*)(sizeof(float) * 3)); // starts 3 floats in (changed from 2 to 3)

	//Get rif of everything

	//***Sets current VAO to nothing
	glBindVertexArray(0); //stop remembering. Changing it's attentions to smw else

	return true;
}

void cVAOMeshManager::m_AppendTextToLastError(std::string text, bool addNewLineBefore ) {

	std::stringstream ssError;
	ssError.str() = this->m_lastErrorString;

	if (addNewLineBefore)
	{
		ssError << std::endl;
	}

	ssError << text;

	this->m_lastErrorString = ssError.str();

	return;
}

 std::string cVAOMeshManager::GetLastError(bool bAndClear /*= true*/) {
	 std::string errorText = this->m_lastErrorString;
	 if (bAndClear) {
		 this->m_lastErrorString = "";
	 }
	 return errorText;
}