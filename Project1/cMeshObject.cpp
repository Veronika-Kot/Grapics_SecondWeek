#include "cMeshObject.h"

cMeshObject::cMeshObject() {
	this->preRotation = glm::vec3(0.0f);
	this->position = glm::vec3(0.0f);
	this->postRotation = glm::vec3(0.0f);
	this->nonUniformScale = glm::vec3(1.0f);
	this->bIsWireFrame = false;
	this->bIsVisiable = true;
	this->uniqueID = nextID;
	nextID++;
	return;
}

unsigned int cMeshObject::nextID = cMeshObject::START;