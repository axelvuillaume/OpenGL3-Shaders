#version 430

in vec3 fragNormal;       // Normal transformée du vertex
in vec3 fragPosition;     // Position transformée du vertex

uniform vec3 lightPos;    // Position de la lumière
uniform vec3 lightColor;  // Couleur de la lumière
uniform vec3 objectColor; // Couleur de l'objet
uniform float shininess;   // Coefficient de brillance

out vec4 FragColor;

void main() {
    // Calcul de l'illumination diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Réduire l'effet d'ombrage pour un éclairage plus uniforme
    float levels = 6.0;  // Augmente le nombre de niveaux pour une lumière plus uniforme
    diff = floor(diff * levels) / levels;

    // Calculer la composante spéculaire
    vec3 viewDir = normalize(-fragPosition); // Direction de la vue (inverse de la position)
    vec3 reflectDir = reflect(-lightDir, norm); // Direction réfléchie
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); // Calcul du terme spéculaire

    // Couleur finale
    vec3 resultColor = diff * lightColor * objectColor; // Éclairage diffuse
    resultColor += 0.2 * lightColor;  // Ajoute une légère lumière ambiante
    resultColor += spec * lightColor; // Ajoute la composante spéculaire
    FragColor = vec4(resultColor, 1.0);
}