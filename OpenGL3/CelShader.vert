#version 430

in vec4 v_coord;          // Coordonn�es du vertex
in vec3 v_normal;         // Normale du vertex
in vec2 v_texcoord;       // Coordonn�es de texture

uniform mat4 MVP;         // Matrice de transformation
uniform mat4 modelMatrix; // Matrice du mod�le
uniform mat3 NM;          // Matrice normale

out vec3 fragNormal;      // Normal transform�e pour le fragment shader
out vec3 fragPosition;    // Position transform�e pour le fragment shader

void main() {
    fragNormal = normalize(NM * v_normal);  // Normal transform�e
    fragPosition = vec3(modelMatrix * v_coord); // Position transform�e
    gl_Position = MVP * v_coord; // Position finale du vertex
}