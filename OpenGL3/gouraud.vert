#version 430

in vec4 v_coord;  // Coordonn�es du vertex
in vec3 v_normal; // Normale du vertex
uniform mat4 MV;  // Matrice ModelView
uniform mat4 MVP; // Matrice Projection * ModelView
uniform mat3 NM;  // Matrice des normales
uniform vec4 Ka;  // Couleur ambiante
uniform vec4 Kd;  // Couleur diffuse
uniform vec4 Ks;  // Couleur sp�culaire
uniform float shininess; // Exposant de brillance
uniform vec3 lightpos;  // Position de la lumi�re

out vec4 color; // Couleur calcul�e pour le fragment

void main()
{ 
    vec3 P = vec3(MV * v_coord);     // Position du vertex
    vec3 N = normalize(NM * v_normal); // Normale transform�e

    // Calculer les vecteurs de lumi�re et de vue
    vec3 L = normalize(lightpos - P); // Vecteur vers la lumi�re
    vec3 V = normalize(-P); // Vecteur vers la cam�ra (peut varier selon la cam�ra)
    vec3 R = reflect(-L, N); // Vecteur r�fl�chi

    // Calculer les termes d'illumination
    // Couleur ambiante
    vec4 amb = Ka; // Couleur ambiante (peut �tre multipli�e par une couleur de lumi�re)

    // Couleur diffuse
    vec4 diff = Kd * max(dot(N, L), 0.0); // Multipli� par la couleur diffuse

    // Couleur sp�culaire
    vec4 spec = Ks * pow(max(dot(R, V), 0.0), shininess); // Multipli� par la couleur sp�culaire

    // Combiner les termes d'illumination
    color = amb + diff + spec; // Couleur finale du vertex
    color = clamp(color, 0.0, 1.0); // S'assurer que la couleur reste entre 0 et 1

    // Calculer la position du vertex dans l'espace de projection
    gl_Position = MVP * v_coord;
}
