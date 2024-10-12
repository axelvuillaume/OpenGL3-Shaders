#version 430
in vec4 v_coord;      // Coordonnées du vertex en vec4
in vec3 v_normal;     // Normales
in vec2 v_texcoord;   // Coordonnées de texture

uniform mat4 MVP;     // Matrice Model-View-Projection
uniform float edgeWidth = 0.06; // Taille du contour

void main() {
    // Conversion des coordonnées du vertex en vec3 pour appliquer la normale
    vec3 position3D = v_coord.xyz; 

    // Agrandir légèrement l'objet le long de la normale pour dessiner le contour
    vec3 enlargedPosition = position3D + v_normal * edgeWidth;

    // Reconstruire un vec4 avec w = 1.0 pour appliquer la transformation MVP
    vec4 pos = MVP * vec4(enlargedPosition, 1.0);

    // Assigner la position finale
    gl_Position = pos;
}