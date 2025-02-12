// OpenGL3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define TITLE "Hello OpenGL!"
#include "formule.h"
#include <chrono>
#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))


int SCREEN_X = 1024;
int SCREEN_Y = 769;
using namespace std;
GLuint phong;
GLuint passthrough;
GLuint gouraud;
GLuint vaoSuzanneCelPostProd;
GLuint vaoSuzanne;
GLuint vaoSuzanneGouraud;
GLuint vaoSuzannePhong;
GLuint vaoSuzanneNormals;
GLuint vbo_mesh_data_normals;
GLuint vaoSuzanneNormal;
vector<glm::vec3> vertex_normals;
GLuint vbo_mesh_data;
vector<GLfloat> suzanne_mesh_data;
GLuint textureBMP;
GLuint celShader;
GLuint border;
GLuint vaoSuzanneBorder;
GLuint vaoSuzanneCelShader;
GLuint shaderTex;
GLuint pixelate;
GLuint normal;
bool framebufferInitialized = false;
GLuint framebuffer, renderedTexture, depthrenderbuffer;
GLuint vaoquad, vboquad;
int TEX_X = SCREEN_X; // Largeur de la texture
int TEX_Y = SCREEN_Y; // Hauteur de la texture
int scene = 0;
float angleX = 0.0f; // rotation around X-axis
float angleY = 0.0f; // rotation around Y-axis
bool isDragging = false; // mouse dragging state
int lastX, lastY;
glm::vec4 Ka(0.1f, 0.1f, 0.1f, 1.0f); // Ambient
glm::vec4 Kd(0.8f, 0.8f, 0.8f, 1.0f); // Diffuse
glm::vec4 Ks(1.0f, 1.0f, 1.0f, 1.0f); // Specular
glm::vec3 lightColor(1.0f, 0.84f, 0.0f);
glm::vec3 objectColor(1.0f, 0.84f, 1.0f);
GLuint postProd;
float shininess = 10.0f; // Shininess
glm::vec3 lightPos(0.0f, 0.0f, 5.0f); // Position de la lumi�re


using namespace std;




float whatTimeIsIt() {
	static auto startTime = std::chrono::high_resolution_clock::now(); // Enregistre le temps de d�part
	auto currentTime = std::chrono::high_resolution_clock::now(); // Obtenir le temps actuel

	// Calculer le temps �coul� en secondes
	std::chrono::duration<float> elapsed = currentTime - startTime;
	return elapsed.count(); // Retourne le temps �coul� en secondes
}


void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		isDragging = (state == GLUT_DOWN);
		lastX = x;
		lastY = y;
	}
}

void mouseMotion(int x, int y) {
	if (isDragging) {
		int deltaX = x - lastX;
		int deltaY = y - lastY;

		// Update angles based on mouse movement
		angleY += deltaX * 0.5f; // adjust sensitivity as needed
		angleX -= deltaY * 0.5f; // adjust sensitivity as needed

		lastX = x; // update last position
		lastY = y;

		// Update the MVP matrix after changing the angles
		//updateMVP();
		glutPostRedisplay(); // request to redraw the window
	}
}

void idle() {
	// Redessiner l'affichage
	glutPostRedisplay();
}


void createFrameBuffer(int X, int Y) {
	// creates a framebuffer of size (X,Y) with one color attachment (texture) and a depth buffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, X, Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Set "renderedTexture" as our color attachment #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	// The depth buffer
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, X, Y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	// check that framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Error creating Framebuffer\n"); exit(EXIT_FAILURE);
	}
}



void updateMVP(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(passthrough, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(passthrough, "NM"), 1, GL_FALSE, &NM[0][0]);
}


void updateMVPNormal(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(normal, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(normal, "NM"), 1, GL_FALSE, &NM[0][0]);
}


void updateMVPGouraud(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(gouraud, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(gouraud, "NM"), 1, GL_FALSE, &NM[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(gouraud, "MV"), 1, GL_FALSE, &MV[0][0]);
}

void updateMVPPhong(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(phong, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(phong, "NM"), 1, GL_FALSE, &NM[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(phong, "MV"), 1, GL_FALSE, &MV[0][0]);
}

void updateMVPBoder(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;

	glUniformMatrix4fv(glGetUniformLocation(border, "MVP"), 1, GL_FALSE, &MVP[0][0]);
}


void updateMVPCelShader(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(celShader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(celShader, "NM"), 1, GL_FALSE, &NM[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(celShader, "modelMatrix"), 1, GL_FALSE, &Model[0][0]);
}

void initScene1()
{
	printf("Scene 1. Qm magnestism Fixed\n");


	//passthoough
	glUseProgram(passthrough);


	
	
	glBindTexture(GL_TEXTURE_2D, textureBMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	

	glm::mat4 Model1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
	Model1 = glm::rotate(Model1, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model1 = glm::rotate(Model1, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MVP1;
	updateMVP(Model1, MVP1);
	glBindVertexArray(vaoSuzanne);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);



	glUseProgram(gouraud);
	glUniform4fv(glGetUniformLocation(gouraud, "Ka"), 1, &Ka[0]);
	glUniform4fv(glGetUniformLocation(gouraud, "Kd"), 1, &Kd[0]);
	glUniform4fv(glGetUniformLocation(gouraud, "Ks"), 1, &Ks[0]);
	glUniform1f(glGetUniformLocation(gouraud, "shininess"), shininess);
	glUniform3fv(glGetUniformLocation(gouraud, "lightpos"), 1, &lightPos[0]);

	glm::mat4 Model2 = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -10.0f));
	Model2 = glm::rotate(Model2, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model2 = glm::rotate(Model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP2;
	updateMVPGouraud(Model2, MVP2);
	glBindVertexArray(vaoSuzanneGouraud);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);

	//glBindVertexArray(vaoSuzanneNormals);
	//glDrawArrays(GL_TRIANGLES, 0, vertex_normals.size() / 9);

	//phong
	glUseProgram(phong);

	// Initialiser les propri�t�s des mat�riaux
	glUniform4fv(glGetUniformLocation(phong, "Ka"), 1, &Ka[0]);
	glUniform4fv(glGetUniformLocation(phong, "Kd"), 1, &Kd[0]);
	glUniform4fv(glGetUniformLocation(phong, "Ks"), 1, &Ks[0]);
	glUniform1f(glGetUniformLocation(phong, "shininess"), shininess);

	// Initialiser la position de la lumi�re et sa couleur
	glUniform3fv(glGetUniformLocation(phong, "lightpos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(phong, "lightColor"), 1, &lightColor[0]);


	glm::mat4 Model3 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, -10.0f));
	Model3 = glm::rotate(Model3, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model3 = glm::rotate(Model3, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP3;
	updateMVPPhong(Model3, MVP3);
	glBindVertexArray(vaoSuzannePhong);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);


	//Cell shading


	//border
	glDepthMask(GL_FALSE);

	glUseProgram(border);
	glm::mat4 Model4 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 3.0f, -10.0f));
	Model4 = glm::rotate(Model4, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model4 = glm::rotate(Model4, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP4;
	updateMVPBoder(Model4, MVP4);

	glBindVertexArray(vaoSuzanneBorder);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);
	glDepthMask(GL_TRUE);

	//celshader
	glUseProgram(celShader);

	glUniform3fv(glGetUniformLocation(celShader, "lightpos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(celShader, "lightColor"), 1, &lightColor[0]);
	glUniform3fv(glGetUniformLocation(celShader, "objectColor"), 1, &objectColor[0]);
	glUniform1f(glGetUniformLocation(celShader, "shininess"), shininess);

	glm::mat4 Model5 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 3.0f, -10.0f));
	Model5 = glm::rotate(Model5, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model5 = glm::rotate(Model5, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP5;
	updateMVPCelShader(Model5, MVP5);

	glBindVertexArray(vaoSuzanneCelShader);
	glCullFace(GL_BACK);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);


	//passthoough
	


	/*
	glBindTexture(GL_TEXTURE_2D, textureBMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/

	glUseProgram(normal);

	glm::mat4 Model6 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, -10.0f));
	Model6 = glm::rotate(Model6, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model6 = glm::rotate(Model6, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MVP6;
	updateMVPNormal(Model6, MVP6);
	glBindVertexArray(vaoSuzanneNormal);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);




}



void initScene2()
{
	printf("Scene 2. Qm magnestism Fixed\n");

	//quad

	glUseProgram(shaderTex); // Utiliser le shader pour le quad
	glUniform1f(glGetUniformLocation(shaderTex, "time"), whatTimeIsIt()); // Passer le temps au shader
	glBindVertexArray(vaoquad); // Lier le VAO du quad
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
void initScene3()
{

	
	printf("Scene 3. Qm magnestism Fixed\n");
	//framebuufer rendu tous ca
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Efface l'�cran et le depth buffer pour cette passe
	glUseProgram(shaderTex);
	glUniform1f(glGetUniformLocation(shaderTex, "time"), whatTimeIsIt());
	glBindVertexArray(vaoquad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Passer 2 : Rendu sur l'�cran sans effacer le contenu pr�c�dent
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(passthrough); // Lier le shader de passe pour Suzanne
	glBindTexture(GL_TEXTURE_2D, renderedTexture); // Lier la texture rendue
	//glUniform1i(glGetUniformLocation(passthrough, "tex"), 0);

	// Calculer la matrice MVP pour Suzanne
	glm::mat4 Model7 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	Model7 = glm::rotate(Model7, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model7 = glm::rotate(Model7, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MVP7;
	updateMVP(Model7, MVP7); // Mettez � jour la matrice MVP

	// Lier le VAO de Suzanne et dessiner
	glBindVertexArray(vaoSuzanne);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);
	

}

void initScene4()
{
	printf("Scene 4. Qm magnestism Fixed\n");
	//PostProd
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(passthrough);

	glBindTexture(GL_TEXTURE_2D, textureBMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Utiliser le shader de l'objet 3D
	glm::mat4 Model8 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	Model8 = glm::rotate(Model8, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model8 = glm::rotate(Model8, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP8;
	updateMVP(Model8, MVP8); // Mettez � jour la matrice MVP
	glBindVertexArray(vaoSuzanne);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);



	//2pass
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pixelate); // Utiliser le shader de post-traitement
	glUniform1f(glGetUniformLocation(pixelate, "pixelSize"), 0.005f);
	glBindTexture(GL_TEXTURE_2D, renderedTexture); // Lier la texture rendue
	glBindVertexArray(vaoquad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Dessiner le quad pour le post-traitement
}

void initScene5()
{
	printf("Scene 5. Qm magnestism Fixed\n");
	//PostProd
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(passthrough);

	glBindTexture(GL_TEXTURE_2D, textureBMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Utiliser le shader de l'objet 3D
	glm::mat4 Model8 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
	Model8 = glm::rotate(Model8, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model8 = glm::rotate(Model8, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP8;
	updateMVP(Model8, MVP8); // Mettez � jour la matrice MVP
	glBindVertexArray(vaoSuzanne);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);



	//2pass
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(postProd); // Utiliser le shader de post-traitement
	glUniform1f(glGetUniformLocation(postProd, "SIZEX"), SCREEN_X);
	glUniform1f(glGetUniformLocation(postProd, "SIZEY"), SCREEN_Y);
	glBindTexture(GL_TEXTURE_2D, renderedTexture); // Lier la texture rendue
	glBindVertexArray(vaoquad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Dessiner le quad pour le post-traitement
}


void cleanup()
{
	glDeleteVertexArrays(1, &vaoSuzanne);
	glDeleteBuffers(1, &vbo_mesh_data);
	glDeleteTextures(1, &renderedTexture);
	glDeleteRenderbuffers(1, &depthrenderbuffer);
	glDeleteFramebuffers(1, &framebuffer);

}

void toggleScene(int s)
{


	//cleanup();
	scene = s;


	switch (scene)
	{
	case 1: initScene1(); break;
	case 2: initScene2(); break;
	case 3: 
		if (!framebufferInitialized) {
			createFrameBuffer(TEX_X, TEX_Y);  // Cr�er une seule fois le framebuffer
			framebufferInitialized = true;
		}
		initScene3();
		break;
	case 4: 
		if (!framebufferInitialized) {
			createFrameBuffer(TEX_X, TEX_Y);
			framebufferInitialized = true;
		}
		initScene4(); 
		break;
	case 5: initScene5(); break;
	}
}



void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	toggleScene(scene);
	

	glutSwapBuffers();
}







void initQuad()
{
	glGenVertexArrays(1, &vaoquad);
	glBindVertexArray(vaoquad);
	const GLfloat vertices[] =
	{ -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	glGenBuffers(1, &vboquad);
	glBindBuffer(GL_ARRAY_BUFFER, vboquad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
}






void init() {
	phong = initShaders("phong.vert", "phong.frag");
	passthrough = initShaders("passthrough.vert", "passthrough.frag");
	gouraud = initShaders("gouraud.vert", "gouraud.frag");
	celShader = initShaders("CelShader.vert","CelShader.frag");
	border = initShaders("border.vert","border.frag");
	shaderTex = initShaders("shaderTex.vert", "shaderTex.frag");
	postProd = initShaders("PostProd.vert", "PostProd.frag");
	pixelate = initShaders("pixelate.vert", "pixelate.frag");
	normal = initShaders("normal.vert", "normal.frag");
	load_obj("suzanne.obj", suzanne_mesh_data);



	textureBMP = loadTextureFromBMP("oui.bmp", 1500, 1144);
	if (textureBMP == 0) {
		std::cout << "Erreur : la texture oui.bmp n'a pas pu �tre charg�e." << std::endl;
	}


	// Charger la texture checker
	GLuint checkerTexture = createTexureChecker();

		
	/*for (size_t i = 0; i < suzanne_mesh_data.size(); ++i) {
		std::cout << suzanne_mesh_data[i] << " ";
	}
	std::cout << std::endl;*/

	std::cout << "Nombre de vertices charg�s : " << suzanne_mesh_data.size() << std::endl;

	//std::cout << "Nombre de normals charg�s : " << vertex_normals.size() << std::endl;

	glEnable(GL_DEPTH_TEST);

	//setup Suzanne
	//vbo
	glGenBuffers(1, &vbo_mesh_data);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_data);
	glBufferData(GL_ARRAY_BUFFER, suzanne_mesh_data.size() * sizeof(GLfloat), &suzanne_mesh_data[0], GL_STATIC_DRAW);

	//vao
	glGenVertexArrays(1, &vaoSuzanne);
	glBindVertexArray(vaoSuzanne);



	//passthrough
	GLuint attribute;
	attribute = glGetAttribLocation(passthrough, "v_coord"); 
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	attribute = glGetAttribLocation(passthrough, "v_texcoord"); 
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	attribute = glGetAttribLocation(passthrough, "v_normal"); 
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		

	//gouraud
	glGenVertexArrays(1, &vaoSuzanneGouraud);
	glBindVertexArray(vaoSuzanneGouraud);

	// Position (v_coord)
	GLuint attribute2;
	attribute2 = glGetAttribLocation(gouraud, "v_coord");
	glEnableVertexAttribArray(attribute2);
	glVertexAttribPointer(attribute2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	// Coordonn�es de texture (v_texcoord)
	attribute2 = glGetAttribLocation(gouraud, "v_texcoord");
	glEnableVertexAttribArray(attribute2);
	glVertexAttribPointer(attribute2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	// Normales (v_normal)
	attribute2 = glGetAttribLocation(gouraud, "v_normal");
	glEnableVertexAttribArray(attribute2);
	glVertexAttribPointer(attribute2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	//phong
	glGenVertexArrays(1, &vaoSuzannePhong);
	glBindVertexArray(vaoSuzannePhong);

	// Position (v_coord)
	GLuint attribute3;
	attribute3 = glGetAttribLocation(phong, "v_coord");
	glEnableVertexAttribArray(attribute3);
	glVertexAttribPointer(attribute3, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	// Coordonn�es de texture (v_texcoord)
	attribute3 = glGetAttribLocation(phong, "v_texcoord");
	glEnableVertexAttribArray(attribute3);
	glVertexAttribPointer(attribute3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	// Normales (v_normal)
	attribute3 = glGetAttribLocation(phong, "v_normal");
	glEnableVertexAttribArray(attribute3);
	glVertexAttribPointer(attribute3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));


	//border

	glGenVertexArrays(1, &vaoSuzanneBorder);
	glBindVertexArray(vaoSuzanneBorder);


	GLuint attribute4;
	attribute4 = glGetAttribLocation(border, "v_coord");
	glEnableVertexAttribArray(attribute4);
	glVertexAttribPointer(attribute4, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	attribute4 = glGetAttribLocation(border, "v_texcoord");
	glEnableVertexAttribArray(attribute4);
	glVertexAttribPointer(attribute4, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	attribute4 = glGetAttribLocation(border, "v_normal");
	glEnableVertexAttribArray(attribute4);
	glVertexAttribPointer(attribute4, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));



	//CelShader

	glGenVertexArrays(1, &vaoSuzanneCelShader);
	glBindVertexArray(vaoSuzanneCelShader);


	GLuint attribute5;
	attribute5 = glGetAttribLocation(celShader, "v_coord");
	glEnableVertexAttribArray(attribute5);
	glVertexAttribPointer(attribute5, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	attribute5 = glGetAttribLocation(celShader, "v_texcoord");
	glEnableVertexAttribArray(attribute5);
	glVertexAttribPointer(attribute5, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	attribute5 = glGetAttribLocation(celShader, "v_normal");
	glEnableVertexAttribArray(attribute5);
	glVertexAttribPointer(attribute5, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));


	//normal

		//vao
	glGenVertexArrays(1, &vaoSuzanneNormal);
	glBindVertexArray(vaoSuzanneNormal);


	GLuint attribute6;
	attribute6 = glGetAttribLocation(normal, "v_coord");
	glEnableVertexAttribArray(attribute6);
	glVertexAttribPointer(attribute6, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	attribute6 = glGetAttribLocation(normal, "v_texcoord");
	glEnableVertexAttribArray(attribute6);
	glVertexAttribPointer(attribute6, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	attribute6 = glGetAttribLocation(normal, "v_normal");
	glEnableVertexAttribArray(attribute6);
	glVertexAttribPointer(attribute6, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));


	////vbonormals
	//glGenBuffers(1, &vbo_mesh_data_normals);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_data_normals);
	//glBufferData(GL_ARRAY_BUFFER, vertex_normals.size() * sizeof(GLfloat), &vertex_normals[0], GL_STATIC_DRAW);


	////vao normals
	//glGenVertexArrays(1, &vaoSuzanneNormals);
	//glBindVertexArray(vaoSuzanneNormals);
		
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0);
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
};











void keyCallback(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': // Augmenter la brillance
		shininess += 0.2f;
		std::cout << "Shininess: " << shininess << std::endl;
		break;
	case 's': // Diminuer la brillance
		shininess = std::max(shininess - 0.2f, 0.0f);
		std::cout << "Shininess: " << shininess << std::endl;
		break;
	case 'a': // D�placer la lumi�re vers la gauche
		lightPos.x -= 0.5f;
		std::cout << "Light position: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << std::endl;
		break;
	case 'd': // D�placer la lumi�re vers la droite
		lightPos.x += 0.5f;
		std::cout << "Light position: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << std::endl;
		break;
	case 'q': // Diminuer la composante ambiante
		Ka -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Ka = glm::max(Ka, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter � un minimum de 0, alpha reste 1.0
		std::cout << "Ambient: (" << Ka.r << ", " << Ka.g << ", " << Ka.b << ")" << std::endl;
		break;
	case 'e': // Augmenter la composante ambiante
		Ka += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Ambient: (" << Ka.r << ", " << Ka.g << ", " << Ka.b << ")" << std::endl;
		break;
	case 'z': // Diminuer la composante diffuse
		Kd -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Kd = glm::max(Kd, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter � un minimum de 0
		std::cout << "Diffuse: (" << Kd.r << ", " << Kd.g << ", " << Kd.b << ")" << std::endl;
		break;
	case 'x': // Augmenter la composante diffuse
		Kd += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Diffuse: (" << Kd.r << ", " << Kd.g << ", " << Kd.b << ")" << std::endl;
		break;
	case 'c': // Diminuer la composante sp�culaire
		Ks -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Ks = glm::max(Ks, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter � un minimum de 0
		std::cout << "Specular: (" << Ks.r << ", " << Ks.g << ", " << Ks.b << ")" << std::endl;
		break;
	case 'v': // Augmenter la composante sp�culaire
		Ks += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Specular: (" << Ks.r << ", " << Ks.g << ", " << Ks.b << ")" << std::endl;
		break;
	case 27: // �chap pour quitter
		exit(0);
		break;
	default:
		break;
	case '1':
		toggleScene(1);
		break;
	case '2':
		toggleScene(2);
		break;
	case '3':
		toggleScene(3);
		break;

	case '4':
		toggleScene(4);
		break;

	case '5':
		toggleScene(5);
		break;

	}


	glutPostRedisplay();
}


int main(int argc, char** argv)
{
    std::cout << "Hello World!\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_X, SCREEN_Y);
	glutCreateWindow(TITLE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);



	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* glewInit failed*/
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));
	initQuad();
	init();
	toggleScene(1);
	glutDisplayFunc(display);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyCallback);
	glutIdleFunc(idle);
	glutMainLoop();
	cleanup();

}