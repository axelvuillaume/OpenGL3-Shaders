#version 430

in vec4 v_coord;          // Coordonnées du vertex
in vec3 v_normal;         // Normale du vertex
in vec2 v_texcoord;       // Coordonnées de texture

uniform mat4 MVP;         // Matrice de transformation
uniform mat4 modelMatrix; // Matrice du modèle
uniform mat3 NM;          // Matrice normale

out vec3 fragNormal;      // Normal transformée pour le fragment shader
out vec3 fragPosition;    // Position transformée pour le fragment shader

void main() {
    fragNormal = normalize(NM * v_normal);  // Normal transformée
    fragPosition = vec3(modelMatrix * v_coord); // Position transformée
    gl_Position = MVP * v_coord; // Position finale du vertex
}