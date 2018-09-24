#include "globalStuff.h"
//#include "linmath.h" //*** replace it with glm

//include glm to draw multiple objects
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <vector>
#include <stdlib.h> //<> are system include
#include <stdio.h>
#include "cShaderManager.h" 
#include <iostream>
#include "cVAOMeshManager.h"
#include "cMeshObject.h"



std::vector<cMeshObject* > vecObjectsToDraw;
///can access vecObjectsToDraw[1] too
cMeshObject* pRogerRabbit = NULL;
glm::vec3 g_CameraEye = glm::vec3(0.0, 0.0, +10.0f);
glm::vec3 g_CameraAt = glm::vec3(0.0, 0.0, 0.0f);
glm::vec3 g_lightPos = glm::vec3(4.0f, 4.0f, 0.0f);
float g_lightBrightness = 4.0f;

cShaderManager* pTheShaderManager; 
cVAOMeshManager* pTheVAOMeshManager;



void LoadModelsIntoScene(void);

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
	GLFWwindow* window; ///creates window of app and free console

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1300, 880, "Banny", NULL, NULL);
	///(W, H, title, ?, ?)

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	//********************************At this point GLFM is happy and has moved to OpenGL***********************************

	//***Create a shader manager
	pTheShaderManager = new cShaderManager();

	cShaderManager::cShader vertexShader;
	cShaderManager::cShader fragmentShader;

	vertexShader.fileName = "vertex01.glsl";
	vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

	fragmentShader.fileName = "fragment01.glsl";
	fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

	if (pTheShaderManager->createProgramFromFile("myShader", vertexShader, fragmentShader)) 
	{
		std::cout << "Compiled shaders OK" << std::endl;
	}
	else {
		std::cout << "OHH No" << std::endl;
		std::cout << pTheShaderManager->getLastError()<< std::endl;
	}
    //***End 

	//***creating a chader program
	GLuint program = pTheShaderManager->getIDFromFriendlyName("myShader"); //glUseProgram(program);...


	//*******Loading Mesh Models
	cVAOMeshManager* pTheVAOMeshManager = new cVAOMeshManager();

	sModelDrawInfo bunnyInfo;
	bunnyInfo.meshFileName = "bun_res3_xyz.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(bunnyInfo, program)) {
		std::cout << "Didn't load the bunny" << std::endl;
		std::cout << pTheShaderManager ->getLastError() << std::endl;
	}
	else {
		std::cout << "Bunny is Loaded" << std::endl;
	}

	sModelDrawInfo airpalneInfo;
	airpalneInfo.meshFileName = "mig29xyz.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(airpalneInfo, program)) {
		std::cout << "Didn't load the airplane 1" << std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}

	sModelDrawInfo fishInfo;
	fishInfo.meshFileName = "PacificCod0.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(fishInfo, program)) {
		std::cout << "Didn't load the fish"<< std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}

	sModelDrawInfo airpalneInfo2;
	airpalneInfo2.meshFileName = "ssj100xyz.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(airpalneInfo2, program)) {
		std::cout << "Didn't load the airplane 2" << std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}

	sModelDrawInfo Utah;
	Utah.meshFileName = "Utah_Teapot.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(Utah, program)) {
		std::cout << "Didn't load the Utah Teapot" << std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}

	sModelDrawInfo terrainInfo;
	terrainInfo.meshFileName = "MeshLab_Fractal_Terrain_xyz.ply";
	if (!pTheVAOMeshManager->LoadModelIntoVAO(terrainInfo, program)) {
		std::cout << "Didn't load the terrain" << std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}

	//****End of loading mesh models

	LoadModelsIntoScene();
	
	//******Loading uniform variables
	GLint objectColor_UniLoc = glGetUniformLocation(program, "objectColor");
	GLint lightPos_UniLoc = glGetUniformLocation(program, "lightPos");
	GLint lightBrightness_UniLoc = glGetUniformLocation(program, "lightBrightness");

	//***Unoform mat MVP
	///GLint mvp_location = glGetUniformLocation(program, "MVP"); //glUniformMatrix4fv(mvp_location,...
	GLint matMoldel_location = glGetUniformLocation(program, "matModel");
	GLint matView_location = glGetUniformLocation(program, "matView");
	GLint matProj_location = glGetUniformLocation(program, "matProj");


	//End of uniforms

	//there is a program logic - constantly refreshing the frame
	while (!glfwWindowShouldClose(window))
	{
		float ratio;
		int width, height;
		glm::mat4x4 m;
		glm::mat4x4 view = glm::mat4(1.0f);
		glm::mat4x4 p;
		glm::mat4x4 mvp;
		///mat4x4 m, p, mvp; linmath model projection model-view-projection

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);

		///Color and depth buffers are two different things
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); ///clear the screen = refresh the frame, clear previous frame

		///Draw all the objects in the sceene
		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vecObjectsToDraw.size();
			objIndex++) 
		{

			//Is this object is visiable

			if (!vecObjectsToDraw[objIndex]->bIsVisiable)
			{
				continue;
			}


			//************************************ glm transformation matrices **************************************

			m = glm::mat4x4(1.0f);		/// mat4x4_identity(m);

			/// Before positioning, rotating around a model's axes
			glm::mat4 preRot_X = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->preRotation.x,
				glm::vec3(1.0f, 0.0, 0.0f));
			m = m * preRot_X;

			glm::mat4 preRot_Y = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->preRotation.y,
				glm::vec3(0.0f, 1.0, 0.0f));
			m = m * preRot_Y;

			glm::mat4 preRot_Z = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->preRotation.z,
				glm::vec3(0.0f, 0.0, 1.0f));
			m = m * preRot_Z;

					//* constantly rotating around the axis
					//glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), //pathing the matrix
					//	(float)glfwGetTime(), //give the timea
					//	glm::vec3(0.0f, 0.0, 1.0f));

					//m = m * rotateZ;

			///Positioning model in the scene
			glm::mat4 matMove = glm::translate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->position);

			m = m * matMove;

			/// After positioning, rotating around the scene's axes
			glm::mat4 postRot_X = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->postRotation.x,
				glm::vec3(1.0f, 0.0, 0.0f));
			m = m * postRot_X;

			glm::mat4 postRot_Y = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->postRotation.y,
				glm::vec3(0.0f, 1.0, 0.0f));
			m = m * postRot_Y;

			glm::mat4 postRot_Z = glm::rotate(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->postRotation.z,
				glm::vec3(0.0f, 0.0, 1.0f));
			m = m * postRot_Z;

			/// And now scale

			glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
				vecObjectsToDraw[objIndex]->nonUniformScale);
			m = m * matScale;

			////**************************** End of transformation matrices *******************************

		
			p = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

			///position 3D camera
			view = glm::lookAt(g_CameraEye, //EyE --- place camera in the world
			g_CameraAt, ///At --- Look at origin
			glm::vec3(0.0f, 1.0f, 0.0f));//UP --- Y axis to be up


			mvp = p * view * m;  /// mat4x4_mul(mvp, p, m); linmath multiplyin all 3 metrisis together 

			glUseProgram(program);
			/*glUniformMatrix4fv(mvp_location, ///glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp); linmath
								1,
								GL_FALSE,
								glm::value_ptr(mvp));*/

			glUniformMatrix4fv(matMoldel_location, 1, GL_FALSE, glm::value_ptr(m));
			glUniformMatrix4fv(matView_location, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(matProj_location, 1, GL_FALSE, glm::value_ptr(p));


			///without below lines modal just filled with surface
			///glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //draw it as a point at each vertex. Reads the data as triangles but draw them as points
			///glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  //draw a line between vertexes, GL_FILL is default

		
			glUniform3f(objectColor_UniLoc,
								vecObjectsToDraw[objIndex]->objColour.r,
								vecObjectsToDraw[objIndex]->objColour.g,
								vecObjectsToDraw[objIndex]->objColour.b);

			 
	
			
			//***assigning unoforms
			glUniform3f(lightPos_UniLoc, g_lightPos.x,
										 g_lightPos.y,
										 g_lightPos.z);

			glUniform1f(lightBrightness_UniLoc, ::g_lightBrightness);

									
			if ( vecObjectsToDraw[objIndex]->bIsWireFrame) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_DEPTH); ///Enables the KEEPING of the depth information
				glDisable(GL_DEPTH_TEST); ///When drawing check the existing depth
				glDisable(GL_CULL_FACE); ///Discared "back facing" triangles 

			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH); ///Enables the KEEPING of the depth information
				glEnable(GL_DEPTH_TEST); ///When drawing check the existing depth
				glEnable(GL_CULL_FACE); ///Discared "back facing" triangles 

			}
			

			//******************Draw mesh models in the screen

			sModelDrawInfo modelInfo;
			modelInfo.meshFileName = vecObjectsToDraw[objIndex]->meshName;

			if (pTheVAOMeshManager->FindDrawInfoByModelName(modelInfo)) 
			{
				//glDrawArrays(GL_TRIANGLES, 0, modelInfo.numberOfIndices);
				glBindVertexArray(modelInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, modelInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray( 0 );
			}
			else {
				std::cout << "Can't draw the mesh" << std::endl;
			}
			//**********************End of drawing

		} // for (unsigned int objIndex = 0; 

		glfwSwapBuffers(window); ///swap windows, so we don't see actual drawing happenin
		glfwPollEvents();

		processKeys(window);

	} //while (!glfwWindowShouldClose(window))

	//****DELETING STUFF

		delete pTheShaderManager;
		delete pTheVAOMeshManager;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


void LoadModelsIntoScene(void) {

	//std::vector<cMeshObject* > vecObjectsToDraw;
	//**** set up some models to draw
	{
		cMeshObject* pMig = new cMeshObject();
		pMig->position = glm::vec3(-3.0f, 0.0f, 0.0f);
		pMig->objColour = glm::vec3(1.0f, 0.0f, 0.0f);
		pMig->nonUniformScale = glm::vec3(0.5f, 0.5f, 0.5f);
		pMig->meshName = "mig29xyz.ply";
		vecObjectsToDraw.push_back(pMig);
	}
	{
		cMeshObject* pFish = new cMeshObject();
		pFish->position = glm::vec3(+3.0f, 0.0f, 0.0f);
		pFish->objColour = glm::vec3(0.0f, 1.1f, 0.0f);
		pFish->meshName = "PacificCod0.ply";
		vecObjectsToDraw.push_back(pFish);
	}
	{
		cMeshObject* pPlain = new cMeshObject();
		pPlain->position = glm::vec3(0.0f, +3.0f, 0.0f);
		pPlain->objColour = glm::vec3(0.0f, 0.0f, 1.1f);
		pPlain->meshName = "ssj100xyz.ply";
		vecObjectsToDraw.push_back(pPlain);
	}
	{
		cMeshObject* pTeaPot = new cMeshObject();
		pTeaPot->position = glm::vec3(0.0f, -3.0f, 0.0f);
		pTeaPot->objColour = glm::vec3(0.19f, 0.6f, 0.3f);
		pTeaPot->nonUniformScale = glm::vec3(0.005f, 0.005f, 0.005f);
		// pTeaPot-> bIsWireFrame = true;
		pTeaPot->meshName = "Utah_Teapot.ply";

		vecObjectsToDraw.push_back(pTeaPot);
	}

	{
		pRogerRabbit = new cMeshObject();
		pRogerRabbit->position = glm::vec3(0.0f, -1.0f, 0.0f);
		pRogerRabbit->objColour = glm::vec3(1.0f, 1.0f, 0.0f);
		pRogerRabbit->nonUniformScale = glm::vec3(3.0f, 3.0f, 3.0f);
		pRogerRabbit->bIsVisiable = true;
		pRogerRabbit->friendlyName = "Roger";
		pRogerRabbit->meshName = "bun_res3_xyz.ply";
		vecObjectsToDraw.push_back(pRogerRabbit);
	}

	{
		cMeshObject* pBunny = new cMeshObject();
		pBunny->position = glm::vec3(0.0f, -1.0f, 0.0f);
		pBunny->objColour = glm::vec3(1.0f, 1.0f, 0.0f);
		pBunny->nonUniformScale = glm::vec3(3.0f, 3.0f, 3.0f);
		pBunny->bIsVisiable = true;
		pBunny->friendlyName = "Bugs";
		pBunny->meshName = "bun_res3_xyz.ply";
		vecObjectsToDraw.push_back(pBunny);
	}

	{
		cMeshObject* pTerrain = new cMeshObject();
		pTerrain->position = glm::vec3(0.0f, -150.0f, 0.0f);
		pTerrain->objColour = glm::vec3(1.0f, 1.0f, 1.0f);
		pTerrain->meshName = "MeshLab_Fractal_Terrain_xyz.ply";
		//pTerrain->nonUniformScale = glm::vec3(0.1f,0.1f,0.1f);
		vecObjectsToDraw.push_back(pTerrain);
	}
	///numberofObjectsToDraw = 5;

	return;
}

cMeshObject* findObjectByFriendlyName(std::string theName) {

	cMeshObject* pTheObjectWeFound = NULL;

	for (unsigned int index = 0; index != vecObjectsToDraw.size(); index++)
	{
		if (vecObjectsToDraw[index]->friendlyName == theName) {
			pTheObjectWeFound = vecObjectsToDraw[index];
		}
	}

	return pTheObjectWeFound;
}
cMeshObject* findObjectByUniqueID(int ID) {
	cMeshObject* pTheObjectWeFound = NULL;

	for (unsigned int index = 0; index != vecObjectsToDraw.size(); index++)
	{
		if (vecObjectsToDraw[index]->getID == ID) {
			pTheObjectWeFound = vecObjectsToDraw[index];
		}
	}

	return pTheObjectWeFound;
}