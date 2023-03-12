//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <fstream>
using namespace std;

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
				glm::vec3(0.0f, 2.0f, 5.5f), 
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.02f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

GLfloat portalAngle;
GLfloat laserZscale=0.0f;
GLfloat laserZtranslate;

gps::Model3D scena;
gps::Model3D spaceshipBody;
gps::Model3D spaceshipWindow;
gps::Model3D c3po;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D portal;
gps::Model3D lake;
gps::Model3D laser;
gps::Model3D stand;
gps::Model3D woodtower;
gps::Model3D woodtower_wrecked;
gps::Model3D dulie;
gps::Model3D bec;
gps::Model3D ochelari;
gps::Model3D rama;
gps::Model3D transporter;
gps::Model3D transporter_window;
gps::Model3D elm;
gps::Model3D chew;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader shaderC3;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;


std::vector<const GLchar*> faces;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

int fogEnable = 1;
int transparent = 1;
float reflEn = 0;

ifstream f("pos.txt");
ifstream g("target.txt");
int prezentare = 0;

GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void initSpotLight_On()
{
	myCustomShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.position"), 0.4f, 3.2f, 10.1f); //pozitia luminii: deasupra turnului
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.direction"), 0.0f, -1.0f, 0.0f); //vreau ca lumina de tip spot sa fie de la position, in jos
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.ambientS"), 0.005f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.diffuseS"), 1.0f, 0.0f, 0.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.specularS"), 4.5f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.cutOff"), glm::cos(glm::radians(-20.0f)));
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.outerCutOff"), glm::cos(glm::radians(30.0f)));
}

void initSpotLight_Off()
{
	myCustomShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.position"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.direction"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.ambientS"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.diffuseS"), 0.0f, 0.0f, 0.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.specularS"), 0.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.cutOff"), glm::cos(glm::radians(0.0f)));
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "lanternLight.outerCutOff"), glm::cos(glm::radians(0.0f)));
}

void initPointLights()
{
	//bec
	myCustomShader.useShaderProgram();
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].position"), -4.172f, 0.48f, -2.275f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].diffuseP"), 1.0f, 0.6f, 0.1f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].specularP"), 5.0f, 2.0f, 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].linear"), 24.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[0].quadratic"), 45.5f);

	//portalSt
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].position"), 5.1f, 0.25f, 2.9f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].diffuseP"), 1.0f, 0.0f, 0.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].specularP"), 3.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].linear"), 1.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].quadratic"), 2.5f);

	//portalDr
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].position"), 5.2f, 0.25f, 5.0f); //pozitia luminii
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].ambientP"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].diffuseP"), 1.0f, 0.0f, 0.0f); //light colour
	glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].specularP"), 3.0f, 0.0f, 0.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].linear"), 3.7f);
	glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].quadratic"), 6.5f);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

bool mouseInit = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400;
float lastY = 300;
float zoom = 45.0f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (mouseInit)
	{
		lastX = xpos;
		lastY = ypos;
		mouseInit = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

gps::MOVE_DIRECTION mouseScrollDirection;

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom -= (float)yoffset; // amount scrolled vertically

	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;

	if (xoffset < yoffset)
		mouseScrollDirection = gps::MOVE_BACKWARD;
	else
		mouseScrollDirection = gps::MOVE_FORWARD;

	myCamera.move(mouseScrollDirection, 0.1f);
}

float intensitate = 0.5f;
int iUp = 1;

void processMovement()
{
	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 0.6f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 0.6f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_F))
	{
		fogEnable = 1;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogEnable"), fogEnable);
		shaderC3.useShaderProgram();
		glUniform1i(glGetUniformLocation(shaderC3.shaderProgram, "fogEnable"), fogEnable);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_G))
	{
		fogEnable = 0;
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogEnable"), fogEnable);
		shaderC3.useShaderProgram();
		glUniform1i(glGetUniformLocation(shaderC3.shaderProgram, "fogEnable"), fogEnable);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_3))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_2))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_1))
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_P])
	{
		portalAngle += 0.3;

		if (myCamera.cameraPosition.x <= 5.7 && myCamera.cameraPosition.x >= 5.2)
		{
			if (myCamera.cameraPosition.y <= 1.49 && myCamera.cameraPosition.y >= 0.02)
			{
				if (myCamera.cameraPosition.z <= 4.8 && myCamera.cameraPosition.z >= 3)
				{
					myCamera = gps::Camera(
						glm::vec3(-22.5f, 1.8f, -8.0f),
						glm::vec3(-16.79f, 1.25f, -17.31f),
						glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}
		}
		myCustomShader.useShaderProgram();
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].diffuseP"), 0.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].diffuseP"), 0.0f, 1.0f, 0.0f);

		if (iUp == 1)
		{
			if (intensitate < 5)
				intensitate += 0.05;
			else
				iUp = 0;
		}
		else
		{
			if (intensitate > 0.5)
				intensitate -= 0.05;
			else
				iUp = 1;
		}

		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].specularP"), 0.0f, intensitate, 0.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].specularP"), 0.0f, intensitate, 0.0f);
	}
	if (!pressedKeys[GLFW_KEY_P])
	{
		myCustomShader.useShaderProgram();
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].diffuseP"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[1].specularP"), 3.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].diffuseP"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "bulbLight[2].specularP"), 3.0f, 0.0f, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_Z])
	{
		if (laserZscale > -0.01f)
			if (laserZscale < 90.4)
				laserZscale += 0.1f;
			else
				if (laserZscale < 90.8f)
					laserZscale += 0.001;
				else
					laserZscale = -0.01f;
		//std::cout << laserZscale << std::endl;
	}
	if (!pressedKeys[GLFW_KEY_Z])
	{
		if (laserZscale > -0.01f)
			laserZscale = 0.0f;
	}

	if (glfwGetKey(glWindow, GLFW_KEY_X))
	{
		initSpotLight_On();
	}

	if (glfwGetKey(glWindow, GLFW_KEY_C))
	{
		initSpotLight_Off();
	}

	if (glfwGetKey(glWindow, GLFW_KEY_8))
	{
		prezentare = 1;
		if (!f.is_open())
			f.open("pos.txt");
		if (!g.is_open())
			g.open("target.txt");
	}

	if (glfwGetKey(glWindow, GLFW_KEY_9))
	{
		prezentare = 0;
		if(f.is_open())
			f.close();
		if(g.is_open())
			g.close();
	}

	if (glfwGetKey(glWindow, GLFW_KEY_I))
	{
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "bulbEn"), 1);
	}

	if (glfwGetKey(glWindow, GLFW_KEY_O))
	{
		myCustomShader.useShaderProgram();
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "bulbEn"), 0);
	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetScrollCallback(glWindow, scrollCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	scena.LoadModel("objects/scena/scena.obj");
	spaceshipBody.LoadModel("objects/spaceship/spaceship_body.obj");
	spaceshipWindow.LoadModel("objects/spaceship/spaceship_window.obj");
	c3po.LoadModel("objects/c3po/c3po.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	portal.LoadModel("objects/portal/portal.obj");
	lake.LoadModel("objects/lake/lake.obj");
	laser.LoadModel("objects/laser/laser.obj");
	stand.LoadModel("objects/c3po/stand.obj");
	woodtower.LoadModel("objects/woodtower/woodtower.obj");
	woodtower_wrecked.LoadModel("objects/woodtower/woodtower_wrecked.obj");
	bec.LoadModel("objects/bec/bec.obj");
	dulie.LoadModel("objects/bec/dulie.obj");
	ochelari.LoadModel("objects/ochelari/ochelari.obj");
	rama.LoadModel("objects/ochelari/rama.obj");
	transporter.LoadModel("objects/transporter/obj/transporter.obj");
	transporter_window.LoadModel("objects/transporter/obj/transporter_window.obj");
	elm.LoadModel("objects/copaci/elm.obj");
	chew.LoadModel("objects/chewbacca_porg/chewbacca.obj");
}

void initShaders() {
	faces.push_back("skybox/posx.jpg"); //right
	faces.push_back("skybox/negx.jpg"); //left
	faces.push_back("skybox/posy.jpg"); //top
	faces.push_back("skybox/negy.jpg"); //bottom
	faces.push_back("skybox/posz.jpg"); //back
	faces.push_back("skybox/negz.jpg"); //front

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	shaderC3.loadShader("shaders/shaderC3.vert", "shaders/shaderC3.frag");
	shaderC3.useShaderProgram();

	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();
}

void initUniforms() {
	shaderC3.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(shaderC3.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(shaderC3.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(shaderC3.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(shaderC3.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(-4.9f, 4.8f, -6.7f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix

	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(-1.43f,0.0f,0.8f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
	
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	
	scena.Draw(shader);

	spaceshipBody.Draw(shader);

	dulie.Draw(shader);

	rama.Draw(shader);

	transporter.Draw(shader);

	elm.Draw(shader);

	chew.Draw(shader);

	if (laserZscale == -0.01f)
	{
		woodtower_wrecked.Draw(shader);
	}
	else
	{
		woodtower.Draw(shader);
	}

	glm::mat4 modelAux = model;

	model = glm::translate(model, glm::vec3(5.48f, 0.5825f, 3.945f));
	model = glm::rotate(model, glm::radians(portalAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-5.48f, -0.5825f, -3.945f));
	

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	portal.Draw(shader);

	model = modelAux;
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	modelAux = model;

	model = glm::translate(model, glm::vec3(0.4636f, 1.549f, -8.593));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, laserZscale));
	model = glm::translate(model, glm::vec3(-0.4636f, -1.549f, 8.593));
	
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	laser.Draw(shader);

	model = modelAux;
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	spaceshipWindow.Draw(shader);
	transporter_window.Draw(shader);
	bec.Draw(shader);
	lake.Draw(shader);

	glDisable(GL_BLEND);
}

void drawC3PO(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	c3po.Draw(shader);
	stand.Draw(shader);
	ochelari.Draw(shader);
}

void renderScene() {
	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);

	drawC3PO(depthMapShader, true);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		//depthMapShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		shaderC3.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderC3.shaderProgram,"view"), 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(glGetUniformLocation(shaderC3.shaderProgram, "lightDir"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shaderC3.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(shaderC3.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		c3po.Draw(shaderC3);
		stand.Draw(shaderC3);
		ochelari.Draw(shaderC3);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);

		skyboxShader.useShaderProgram();
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
			glm::value_ptr(view));

		projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
			glm::value_ptr(projection));

		mySkyBox.Draw(skyboxShader, view, projection);

	}
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	
	initPointLights();
	
	initFBO();
	
	glCheckError();

	//glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		

		//f << myCamera.cameraPosition.x << " " << myCamera.cameraPosition.y << " " << myCamera.cameraPosition.z << endl;
		//g << myCamera.cameraFrontDirection.x << " " << myCamera.cameraFrontDirection.y << " " << myCamera.cameraFrontDirection.z << endl;

		if (prezentare == 1)
		{
			f >> myCamera.cameraPosition.x;
			f >> myCamera.cameraPosition.y;
			f >> myCamera.cameraPosition.z;

			g >> myCamera.cameraFrontDirection.x;
			g >> myCamera.cameraFrontDirection.y;
			g >> myCamera.cameraFrontDirection.z;
		}

		renderScene();		
		
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
