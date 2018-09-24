//globalStuff.h
#ifndef _globalStuff_HG_
#define _globalStuff_HG_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include "sPlyVertex.h"
#include "sPlyTriangle.h"
#include "cMeshObject.h"

extern cMeshObject* pRogerRabbit;

extern glm::vec3 g_CameraEye;
extern glm::vec3 g_CameraAt;

extern glm::vec3 g_lightPos;
extern float g_lightBrightness;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processKeys(GLFWwindow* window);

cMeshObject* findObjectByFriendlyName(std::string theName);
cMeshObject* findObjectByUniqueID(int ID);


//extern key means "Don't allocate it here, bc it is(will be) allocated somewhere else,
//It helps to avoid Linker error. "extern" doesn't work if assign to 0
//1.Always put extern in front of .h variables
//2.NEVER assigning it to anyting 


//OLD GLOBAL STUFF
//extern sPlyVertex* g_pArrayVert;  //g for global
//extern unsigned int g_numberOfVertices;
//extern sPlyTriangle* g_pArrayTris; //or  NULL or nullptr
//extern unsigned int g_numberOfTriangles;
//extern unsigned int g_UnrolledNumberOfVertices;
//bool LoadPlyFileData(std::string fileName);
//bool LoadMeshIntoGPUBuffer(void);
//bool Load_And_Unroll_MeshIntoGPUBuffer(void);
//END OF THE OLD GLOBAL STUFF

#endif
