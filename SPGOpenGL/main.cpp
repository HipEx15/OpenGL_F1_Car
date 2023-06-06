#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <stack>

#include "objloader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI glm::pi<float>()

GLuint shader_programme;
glm::mat4 projectionMatrix, viewMatrix, modelMatrix1, modelMatrix2;
std::stack<glm::mat4> modelStack;

//Obiect 1

std::vector< glm::vec3 > vertices1Normals;
GLuint vaoObj1, vboObj1;
std::vector< glm::vec3 > vertices1;
std::vector< glm::vec3 > uvs1;
std::vector< glm::vec3 > normals1;

//Obiect 2

std::vector< glm::vec3 > vertices2Normals;
GLuint vaoObj2, vboObj2;
std::vector< glm::vec3 > vertices2;
std::vector< glm::vec3 > uvs2;
std::vector< glm::vec3 > normals2;

//Lighting
glm::vec3 lightPos1(200, 200, -50);
glm::vec3 lightPos2(-200, 150, 0);

//Camera
glm::vec3 cameraPosition = glm::vec3(0.0f, 60.0f, 0.0f);
glm::vec3 cameraObj; 

float cameraY = 0.0f; //giratie
float cameraX = 0.0f; // tangaj
float cameraSpeed = 1.25f;

bool mouseCliked = true;
float MouseX = 0.0f;
float MouseY = 0.0f;

float axisRotAngle = PI/2 ;
float radius = 2;

float scaleFactor = 0.5; //scale factor pentru obiect
float scaleFactorGarage = 50.0; //scale factor pentru garaj

//Coordonate obiecte
glm::vec3 objectPosition(0, 50, 0);
glm::vec3 garagePosition(0, 0.5, 0);

//Texturi obiecte
unsigned int texture_1;
unsigned int texture_2;

/// <summary>
/// Erori shader
/// </summary>
/// <param name="obj"></param>
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

/// <summary>
/// Erori program
/// </summary>
/// <param name="obj"></param>
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

/// <summary>
/// Citire din fisier
/// </summary>
/// <param name="fn"></param>
/// <returns></returns>
std::string textFileRead(char* fn)
{
	std::ifstream ifile(fn);
	std::string filetext;
	while (ifile.good())
	{
		std::string line;
		std::getline(ifile, line);
		filetext.append(line + "\n");
	}
	return filetext;
}

/// <summary>
/// Incarcare obiectul 1
/// </summary>
void loadMesh1() {
	bool res = loadOBJ("obj/Formula_1.obj", vertices1, uvs1, normals1);

	for (int i = 0; i < vertices1.size(); i++)
	{
		vertices1Normals.push_back(vertices1[i]);
		vertices1Normals.push_back(normals1[i]);
		vertices1Normals.push_back(uvs1[i]);
	}
}

/// <summary>
/// Incarcare obiectul 2
/// </summary>
void loadMesh2() {
	bool res = loadOBJ("obj/building.obj", vertices2, uvs2, normals2);

	
	for (int i = 0; i < vertices2.size(); i++)
	{
		vertices2Normals.push_back(vertices2[i]);
		vertices2Normals.push_back(normals2[i]);
		vertices2Normals.push_back(uvs2[i]);
	}
}

/// <summary>
/// Incarcare textura obiectul 1
/// </summary>
void loadTexture1() 
{
	glGenTextures(1, &texture_1);
	glBindTexture(GL_TEXTURE_2D, texture_1);

	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("image.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint textureUniformLoc = glGetUniformLocation(shader_programme, "texture_1");
	glUniform1i(textureUniformLoc, 0); 
}

/// <summary>
/// Incarcare textura obiectul 2
/// </summary>
void loadTexture2() 
{
	glGenTextures(1, &texture_2);
	glBindTexture(GL_TEXTURE_2D, texture_2);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("material_diffuse.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 1);

	GLuint textureUniformLoc = glGetUniformLocation(shader_programme, "texture_2");
	glUniform1i(textureUniformLoc, 1);
}

/// <summary>
/// Actualizarea coordonatelor camerei
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void mouseMovement(int x, int y)
{
	float cameraSensitivity = 0.1f;

	if (mouseCliked)
	{
		MouseX = x;
		MouseY = y;
		mouseCliked = false;
	}

	float xOffset = x - MouseX;
	float yOffset = y - MouseY;
	MouseX = x;
	MouseY = y;

	xOffset *= cameraSensitivity;
	yOffset *= cameraSensitivity;

	cameraY += xOffset;
	cameraX += yOffset;

	if (cameraX > 89.0f)
		cameraX = 89.0f;
	if (cameraX < -89.0f)
		cameraX = -89.0f;

	cameraObj = glm::vec3(
		cos(glm::radians(cameraY)) * cos(glm::radians(cameraX)),
		sin(glm::radians(cameraX)),
		sin(glm::radians(cameraY)) * cos(glm::radians(cameraX))
	);

	glutPostRedisplay();
}


/// <summary>
/// Initializare OPENGL
/// </summary>
void init()
{
	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION);   
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 0);

	glewInit();

	loadMesh1();

	glGenBuffers(1, &vboObj1);
	glBindBuffer(GL_ARRAY_BUFFER, vboObj1);
	glBufferData(GL_ARRAY_BUFFER, vertices1Normals.size() * sizeof(glm::vec3), &vertices1Normals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoObj1);
	glBindVertexArray(vaoObj1);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), NULL);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 6));

	loadMesh2();

	glGenBuffers(1, &vboObj2);
	glBindBuffer(GL_ARRAY_BUFFER, vboObj2);
	glBufferData(GL_ARRAY_BUFFER, vertices2Normals.size() * sizeof(glm::vec3), &vertices2Normals[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vaoObj2);
	glBindVertexArray(vaoObj2);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), NULL);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(sizeof(float) * 6));

	loadTexture1();
	loadTexture2();

	std::string vstext = textFileRead("vertex.vert");
	std::string fstext = textFileRead("fragment.frag");
	const char* vertex_shader = vstext.c_str();
	const char* fragment_shader = fstext.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	int success;
	char infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	glutPassiveMotionFunc(mouseMovement);

	printShaderInfoLog(fs);
	printShaderInfoLog(vs);
	printProgramInfoLog(shader_programme);
}

/// <summary>
/// Afisarea obiectelor pe ecran
/// </summary>
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_programme);
	glClearColor(0.651, 0.839, 0.875, 1);

	glBindVertexArray(vaoObj1);

	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + glm::normalize(cameraObj), glm::vec3(0.0f, 1.0f, 0.0f));

	GLuint lightPosLoc1 = glGetUniformLocation(shader_programme, "lightPos1");
	glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));

	GLuint lightPosLoc2 = glGetUniformLocation(shader_programme, "lightPos2");
	glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));

	GLuint viewPosLoc = glGetUniformLocation(shader_programme, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPosition));

	modelMatrix1 = glm::mat4();
	modelMatrix1 *= glm::scale(glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	modelMatrix1 *= glm::translate(objectPosition);

	modelMatrix2 = glm::mat4();
	modelMatrix2 *= glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix2 *= glm::scale(glm::vec3(scaleFactorGarage, scaleFactorGarage, scaleFactorGarage));
	modelMatrix2 *= glm::translate(garagePosition);

	GLuint uniformLocation = glGetUniformLocation(shader_programme, "objectID");

	// Obiect 1
	int objectID = 1;
	glUniform1i(uniformLocation, objectID);

	GLuint modelMatrixLoc1 = glGetUniformLocation(shader_programme, "modelMatrix");
	glUniformMatrix4fv(modelMatrixLoc1, 1, GL_FALSE, glm::value_ptr(modelMatrix1));

	GLuint modelMatrix1Loc1 = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrix1Loc1, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix1));

	glm::mat4 normalMatrix1 = glm::transpose(glm::inverse(modelMatrix1));
	GLuint normalMatrix1Loc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrix1Loc, 1, GL_FALSE, glm::value_ptr(normalMatrix1));

	// Textura obiect 1
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_1);

	GLuint textureLoc1 = glGetUniformLocation(shader_programme, "texture_1");
	glUniform1i(textureLoc1, 0);

	glDrawArrays(GL_TRIANGLES, 0, vertices1Normals.size());

	// Obiect 2

	glBindVertexArray(vaoObj2);

	objectID = 2;
	glUniform1i(uniformLocation, objectID);

	GLuint modelMatrixLoc2 = glGetUniformLocation(shader_programme, "modelMatrix");
	glUniformMatrix4fv(modelMatrixLoc2, 1, GL_FALSE, glm::value_ptr(modelMatrix2));

	GLuint modelMatrix1Loc2 = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix");
	glUniformMatrix4fv(modelMatrix1Loc2, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix2));

	glm::mat4 normalMatrix2 = glm::transpose(glm::inverse(modelMatrix2));
	GLuint normalMatrix2Loc = glGetUniformLocation(shader_programme, "normalMatrix");
	glUniformMatrix4fv(normalMatrix2Loc, 1, GL_FALSE, glm::value_ptr(normalMatrix2));

	// Textura obiect 2
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_2);

	GLuint textureLoc2 = glGetUniformLocation(shader_programme, "texture_2");
	glUniform1i(textureLoc2, 1);

	glDrawArrays(GL_TRIANGLES, 0, vertices2Normals.size());

	glutSwapBuffers();
	glutPostRedisplay();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	projectionMatrix = glm::perspective(PI / 4, (float)w / h, 1.0f, 1000.0f);
}

/// <summary>
/// Controlul camerei
/// </summary>
/// <param name="key"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		scaleFactor += 0.01;
		break;

	case '-':
		scaleFactor -= 0.01;
		break;

	case 'w':
		cameraPosition += cameraSpeed * glm::normalize(cameraObj);
		break;
	case 's':
		cameraPosition -= cameraSpeed * glm::normalize(cameraObj);
		break;
	case 'a':
		cameraPosition -= glm::normalize(glm::cross(
			cameraObj,
			glm::vec3(0.0f, 1.0f, 0.0f)
		)) * cameraSpeed;
		break;
	case 'd':
		cameraPosition += glm::normalize(glm::cross(
			cameraObj,
			glm::vec3(0.0f, 1.0f, 0.0f)
		)) * cameraSpeed;
		break;

	case 'e':
		cameraY += axisRotAngle;
		cameraObj = glm::vec3(
			cos(glm::radians(cameraY)) * cos(glm::radians(cameraX)),
			sin(glm::radians(cameraX)),
			sin(glm::radians(cameraY)) * cos(glm::radians(cameraX))
		);
		break;

	case 'q':
		cameraY -= axisRotAngle;
		cameraObj = glm::vec3(
			cos(glm::radians(cameraY)) * cos(glm::radians(cameraX)),
			sin(glm::radians(cameraX)),
			sin(glm::radians(cameraY)) * cos(glm::radians(cameraX))
		);
		break;


	case 't':
		objectPosition.y += 0.25f;
		break;
	case 'g':
		objectPosition.y -= 0.25f;
		break;
	case 'f':
		objectPosition.x -= 1.0f;
		break;
	case 'h':
		objectPosition.x += 1.0f;
		break;
	case 'r':
		objectPosition.z -= 1.0f;
		break;
	case 'y':
		objectPosition.z += 1.0f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024, 720);
	glutCreateWindow("Formula 1");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}