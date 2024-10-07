#include "formule.h"



GLuint loadTextureFromBMP(const char* filename, int width, int height) {
	GLuint texture = 0; unsigned char* data;
	FILE* file; errno_t err;
	if ((err = fopen_s(&file, filename, "rb")) != 0) printf("Error: Texture was not opened.\n");
	else {
		data = (unsigned char*)malloc(width * height * 3);
		fread(data, width * height * 3, 1, file);
		fclose(file);
		for (int i = 0; i < width * height; ++i) { // bmp files are encoded BGR and not RGB
			int index = i * 3; unsigned char B, R;
			B = data[index]; R = data[index + 2];
			data[index] = R; data[index + 2] = B;
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(data);
	}
	return texture;
}

GLuint createTexureChecker() {
	const int CHECKERSIZE = 256;
	float* checkerImage = new float[CHECKERSIZE * CHECKERSIZE * 4];
	int i, j; float c;
	for (i = 0; i < CHECKERSIZE; i++)
		for (j = 0; j < CHECKERSIZE; j++) {
			c = (((i & 0x8) == 0) ^ ((j & 0x8)) == 0); // one square = 8 pixels
			checkerImage[4 * (i * CHECKERSIZE + j)] = (GLfloat)c;
			checkerImage[4 * (i * CHECKERSIZE + j) + 1] = (GLfloat)c;
			checkerImage[4 * (i * CHECKERSIZE + j) + 2] = (GLfloat)c;
			checkerImage[4 * (i * CHECKERSIZE + j) + 3] = (GLfloat)1.0f;
		}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERSIZE, CHECKERSIZE, 0, GL_RGBA, GL_FLOAT, checkerImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete[] checkerImage;
	return texture;
}


GLchar* readShaderSource(const char* shaderFile)
{
	FILE* fp;
	fopen_s(&fp, shaderFile, "r");
	GLchar* buf;
	long size;
	if (fp == NULL) return NULL;
	fseek(fp, 0L, SEEK_END);//go to end
	size = ftell(fp); //get size
	fseek(fp, 0L, SEEK_SET);//go to beginning
	buf = (GLchar*)malloc((size + 1) * sizeof(GLchar));
	fread(buf, 1, size, fp);
	buf[size] = 0;
	fclose(fp);
	return buf;

}

GLuint initShaders(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
		const char* filename;
		GLenum type;
		GLchar* source;
	} shaders[2] = {
	{ vShaderFile, GL_VERTEX_SHADER, NULL },
	{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};
	GLuint program = glCreateProgram();
	for (int i = 0; i < 2; ++i) {
		Shader& s = shaders[i];
		s.source = readShaderSource(s.filename);
		if (shaders[i].source == NULL) {
			printf("Failed to read %s\n", s.filename);
			exit(EXIT_FAILURE);
		}
		GLuint shader = glCreateShader(s.type);
		glShaderSource(shader, 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shader);
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			printf("%s failed to compile:\n", s.filename);
			GLint logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			printf("%s\n", logMsg);
			delete[] logMsg;
			exit(EXIT_FAILURE);
		}
		delete[] s.source;
		glAttachShader(program, shader);
	}
	/* link and error check */
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		printf("Shader program failed to link:\n");
		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		printf("%s\n", logMsg);
		delete[] logMsg;
		exit(EXIT_FAILURE);
	}
	/* use program object */
	(program);
	return program;
};



void load_obj(const char* filename, vector<GLfloat>& mesh_data) {
	vector<glm::vec4> v;  // Vertex positions
	vector<glm::vec2> vt; // Texture coordinates
	vector<glm::vec3> vn; // Vertex normals
	vector<vector<GLushort>> f; // Face indices (vertex, texture, normal)

	// Open the file
	ifstream in(filename, ios::in);
	if (!in) {
		cerr << "Cannot open " << filename << endl;
		exit(1);
	}

	string line;
	while (getline(in, line)) {
		// Vertex positions
		if (line.substr(0, 2) == "v ") {
			istringstream s(line.substr(2));
			glm::vec4 v4;
			s >> v4.x; s >> v4.y; s >> v4.z; v4.w = 1.0f; // w is 1 for 3D vertex
			v.push_back(v4);
		}
		// Texture coordinates
		else if (line.substr(0, 3) == "vt ") {
			istringstream s(line.substr(3));
			glm::vec2 vt2;
			s >> vt2.x; s >> vt2.y;
			vt.push_back(vt2);
		}
		// Vertex normals
		else if (line.substr(0, 3) == "vn ") {
			istringstream s(line.substr(3));
			glm::vec3 vn3;
			s >> vn3.x; s >> vn3.y; s >> vn3.z;
			vn.push_back(vn3);
		}
		// Face data
		else if (line.substr(0, 2) == "f ") {
			vector<GLushort> face;
			istringstream s(line.substr(2));
			string vert_info;
			while (s >> vert_info) {
				stringstream vert_stream(vert_info);
				string v_idx_str, vt_idx_str, vn_idx_str;
				GLushort v_idx, vt_idx = 0, vn_idx = 0;

				getline(vert_stream, v_idx_str, '/');
				v_idx = stoi(v_idx_str) - 1; // OBJ indices start at 1

				if (getline(vert_stream, vt_idx_str, '/')) {
					if (!vt_idx_str.empty()) vt_idx = stoi(vt_idx_str) - 1;
				}
				if (getline(vert_stream, vn_idx_str)) {
					if (!vn_idx_str.empty()) vn_idx = stoi(vn_idx_str) - 1;
				}

				face.push_back(v_idx);  // Store vertex index
				face.push_back(vt_idx); // Store texture coordinate index
				face.push_back(vn_idx); // Store normal index
			}
			f.push_back(face);
		}
	}

	// Containers for vertex normals and normal counts (to average them later)
	vector<glm::vec3> vertex_normals(v.size(), glm::vec3(0.0f));
	vector<int> vertex_normal_count(v.size(), 0);

	// Accumulate normals for each vertex
	for (const auto& face : f) {
		glm::vec3 face_normal(0.0f, 0.0f, 0.0f);

		// Get the normal for the first vertex of the face (assumes flat shading)
		if (face.size() >= 3 && face[2] < vn.size()) {
			face_normal = vn[face[2]];
		}

		// Accumulate the normal for each vertex in the face
		for (size_t i = 0; i < face.size(); i += 3) {
			GLushort v_idx = face[i];
			vertex_normals[v_idx] += face_normal;
			vertex_normal_count[v_idx]++;
		}
	}

	// Average the normals
	for (size_t i = 0; i < vertex_normals.size(); ++i) {
		if (vertex_normal_count[i] > 0) {
			vertex_normals[i] /= float(vertex_normal_count[i]);
			vertex_normals[i] = glm::normalize(vertex_normals[i]);
		}
	}

	// Process faces into mesh_data
	for (const auto& face : f) {
		for (size_t i = 0; i < face.size(); i += 3) {
			GLushort v_idx = face[i];
			GLushort vt_idx = face[i + 1];

			// Push vertex position
			mesh_data.push_back(v[v_idx].x);
			mesh_data.push_back(v[v_idx].y);
			mesh_data.push_back(v[v_idx].z);
			mesh_data.push_back(v[v_idx].w);

			// Push texture coordinates (if they exist)
			if (vt_idx < vt.size()) {
				mesh_data.push_back(vt[vt_idx].x);
				mesh_data.push_back(vt[vt_idx].y);
			}
			else {
				mesh_data.push_back(0.0f); // Default if no texture
				mesh_data.push_back(0.0f);
			}

			// Push averaged vertex normals
			mesh_data.push_back(vertex_normals[v_idx].x);
			mesh_data.push_back(vertex_normals[v_idx].y);
			mesh_data.push_back(vertex_normals[v_idx].z);
		}
	}
}