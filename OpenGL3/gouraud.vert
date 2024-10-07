#version 430

in vec4 v_coord;  // Coordonnées du vertex
in vec3 v_normal; // Normale du vertex
uniform mat4 MV;  // Matrice ModelView
uniform mat4 MVP; // Matrice Projection * ModelView
uniform mat3 NM;  // Matrice des normales
uniform vec4 Ka;  // Couleur ambiante
uniform vec4 Kd;  // Couleur diffuse
uniform vec4 Ks;  // Couleur spéculaire
uniform float shininess; // Exposant de brillance
uniform vec3 lightpos;  // Position de la lumière

out vec4 color; // Couleur calculée pour le fragment

void main()
{ 
    vec3 P = vec3(MV * v_coord);     // Position du vertex
    vec3 N = normalize(NM * v_normal); // Normale transformée

    // Calculer les vecteurs de lumière et de vue
    vec3 L = normalize(lightpos - P); // Vecteur vers la lumière
    vec3 V = normalize(-P); // Vecteur vers la caméra (peut varier selon la caméra)
    vec3 R = reflect(-L, N); // Vecteur réfléchi

    // Calculer les termes d'illumination
    // Couleur ambiante
    vec4 amb = Ka; // Couleur ambiante (peut être multipliée par une couleur de lumière)

    // Couleur diffuse
    vec4 diff = Kd * max(dot(N, L), 0.0); // Multiplié par la couleur diffuse

    // Couleur spéculaire
    vec4 spec = Ks * pow(max(dot(R, V), 0.0), shininess); // Multiplié par la couleur spéculaire

    // Combiner les termes d'illumination
    color = amb + diff + spec; // Couleur finale du vertex
    color = clamp(color, 0.0, 1.0); // S'assurer que la couleur reste entre 0 et 1

    // Calculer la position du vertex dans l'espace de projection
    gl_Position = MVP * v_coord;
}
