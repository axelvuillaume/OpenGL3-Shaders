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
#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))


int SCREEN_X = 1024;
int SCREEN_Y = 769;
using namespace std;
GLuint phong;
GLuint passthrough;
GLuint gouraud;
GLuint vaoSuzanne;
GLuint vaoSuzanneGouraud;
GLuint vaoSuzannePhong;
GLuint vaoSuzanneNormals;
GLuint vbo_mesh_data_normals;
vector<glm::vec3> vertex_normals;
GLuint vbo_mesh_data;
vector<GLfloat> suzanne_mesh_data;
float angleX = 0.0f; // rotation around X-axis
float angleY = 0.0f; // rotation around Y-axis
bool isDragging = false; // mouse dragging state
int lastX, lastY;
glm::vec4 Ka(0.1f, 0.1f, 0.1f, 1.0f); // Ambient
glm::vec4 Kd(0.8f, 0.8f, 0.8f, 1.0f); // Diffuse
glm::vec4 Ks(1.0f, 1.0f, 1.0f, 1.0f); // Specular
glm::vec3 lightColor(0.0f, 1.0f, 1.0f);
float shininess = 10.0f; // Shininess
glm::vec3 lightPos(0.0f, 0.0f, 5.0f); // Position de la lumière


using namespace std;

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
	case 'a': // Déplacer la lumière vers la gauche
		lightPos.x -= 0.5f;
		std::cout << "Light position: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << std::endl;
		break;
	case 'd': // Déplacer la lumière vers la droite
		lightPos.x += 0.5f;
		std::cout << "Light position: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << std::endl;
		break;
	case 'q': // Diminuer la composante ambiante
		Ka -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Ka = glm::max(Ka, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter à un minimum de 0, alpha reste 1.0
		std::cout << "Ambient: (" << Ka.r << ", " << Ka.g << ", " << Ka.b << ")" << std::endl;
		break;
	case 'e': // Augmenter la composante ambiante
		Ka += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Ambient: (" << Ka.r << ", " << Ka.g << ", " << Ka.b << ")" << std::endl;
		break;
	case 'z': // Diminuer la composante diffuse
		Kd -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Kd = glm::max(Kd, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter à un minimum de 0
		std::cout << "Diffuse: (" << Kd.r << ", " << Kd.g << ", " << Kd.b << ")" << std::endl;
		break;
	case 'x': // Augmenter la composante diffuse
		Kd += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Diffuse: (" << Kd.r << ", " << Kd.g << ", " << Kd.b << ")" << std::endl;
		break;
	case 'c': // Diminuer la composante spéculaire
		Ks -= glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		Ks = glm::max(Ks, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Limiter à un minimum de 0
		std::cout << "Specular: (" << Ks.r << ", " << Ks.g << ", " << Ks.b << ")" << std::endl;
		break;
	case 'v': // Augmenter la composante spéculaire
		Ks += glm::vec4(0.05f, 0.05f, 0.05f, 0.0f);
		std::cout << "Specular: (" << Ks.r << ", " << Ks.g << ", " << Ks.b << ")" << std::endl;
		break;
	case 27: // Échap pour quitter
		exit(0);
		break;
	default:
		break;
	}


	glutPostRedisplay();
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



void updateMVP(glm::mat4& Model, glm::mat4& MVP) {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / (float)SCREEN_Y, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MV = View * Model;
	MVP = Projection * MV;
	glm::mat3 NM = glm::transpose(glm::inverse(glm::mat3(MV)));

	glUniformMatrix4fv(glGetUniformLocation(passthrough, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix3fv(glGetUniformLocation(passthrough, "NM"), 1, GL_FALSE, &NM[0][0]);
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

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(passthrough);
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

	glUseProgram(phong);

	// Initialiser les propriétés des matériaux
	glUniform4fv(glGetUniformLocation(phong, "Ka"), 1, &Ka[0]);
	glUniform4fv(glGetUniformLocation(phong, "Kd"), 1, &Kd[0]);
	glUniform4fv(glGetUniformLocation(phong, "Ks"), 1, &Ks[0]);
	glUniform1f(glGetUniformLocation(phong, "shininess"), shininess);

	// Initialiser la position de la lumière et sa couleur
	glUniform3fv(glGetUniformLocation(phong, "lightpos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(phong, "lightColor"), 1, &lightColor[0]);


	glm::mat4 Model3 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, -10.0f));
	Model3 = glm::rotate(Model3, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
	Model3 = glm::rotate(Model3, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP3;
	updateMVPGouraud(Model3, MVP3);
	glBindVertexArray(vaoSuzannePhong);
	glDrawArrays(GL_TRIANGLES, 0, suzanne_mesh_data.size() / 9);


	glColor3f(1.0f, 0.0f, 0.0f);


	glutSwapBuffers();
}


void init() {
	phong = initShaders("phong.vert", "phong.frag");
	passthrough = initShaders("passthrough.vert", "passthrough.frag");
	gouraud = initShaders("gouraud.vert", "gouraud.frag");
	load_obj("suzanne.obj", suzanne_mesh_data);



	GLuint textureBMP = loadTextureFromBMP("oui.jpg", 256, 256);
	// Appliquer GL_CLAMP_TO_EDGE pour cette texture
	glBindTexture(GL_TEXTURE_2D, textureBMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Charger la texture checker
	GLuint checkerTexture = createTexureChecker();

	


		
	/*for (size_t i = 0; i < suzanne_mesh_data.size(); ++i) {
		std::cout << suzanne_mesh_data[i] << " ";
	}
	std::cout << std::endl;*/

	std::cout << "Nombre de vertices chargés : " << suzanne_mesh_data.size() << std::endl;

	//std::cout << "Nombre de normals chargés : " << vertex_normals.size() << std::endl;

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

	// Coordonnées de texture (v_texcoord)
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

	// Coordonnées de texture (v_texcoord)
	attribute3 = glGetAttribLocation(phong, "v_texcoord");
	glEnableVertexAttribArray(attribute3);
	glVertexAttribPointer(attribute3, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	// Normales (v_normal)
	attribute3 = glGetAttribLocation(phong, "v_normal");
	glEnableVertexAttribArray(attribute3);
	glVertexAttribPointer(attribute3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));


	
	////vbonormals
	//glGenBuffers(1, &vbo_mesh_data_normals);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_data_normals);
	//glBufferData(GL_ARRAY_BUFFER, vertex_normals.size() * sizeof(GLfloat), &vertex_normals[0], GL_STATIC_DRAW);


	////vao normals
	//glGenVertexArrays(1, &vaoSuzanneNormals);
	//glBindVertexArray(vaoSuzanneNormals);
		
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -2.0, 2.0);
	glViewport(0, 0, SCREEN_X, SCREEN_Y);
};


void cleanup()
{
		glDeleteVertexArrays(1, &vaoSuzanne);
		glDeleteBuffers(1, &vbo_mesh_data);
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
	init();
	glutDisplayFunc(display);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyCallback);
	glutMainLoop();
	cleanup();
}