#pragma once
#define FORMULE_H
#ifdef FORMULE_H

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;



GLuint initShaders(const char* vShaderFile, const char* fShaderFile);
GLuint loadTextureFromBMP(const char* filename, int width, int height);
GLchar* readShaderSource(const char* shaderFile);
GLuint createTexureChecker();
void load_obj(const char* filename, vector<GLfloat>& mesh_data);
#endif // FORMULE_H
