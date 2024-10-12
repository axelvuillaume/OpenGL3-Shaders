#version 430

in vec3 fragNormal;       // Normal transform�e du vertex
in vec3 fragPosition;     // Position transform�e du vertex

uniform vec3 lightPos;    // Position de la lumi�re
uniform vec3 lightColor;  // Couleur de la lumi�re
uniform vec3 objectColor; // Couleur de l'objet
uniform float shininess;   // Coefficient de brillance

out vec4 FragColor;

void main() {
    // Calcul de l'illumination diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // R�duire l'effet d'ombrage pour un �clairage plus uniforme
    float levels = 6.0;  // Augmente le nombre de niveaux pour une lumi�re plus uniforme
    diff = floor(diff * levels) / levels;

    // Calculer la composante sp�culaire
    vec3 viewDir = normalize(-fragPosition); // Direction de la vue (inverse de la position)
    vec3 reflectDir = reflect(-lightDir, norm); // Direction r�fl�chie
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); // Calcul du terme sp�culaire

    // Couleur finale
    vec3 resultColor = diff * lightColor * objectColor; // �clairage diffuse
    resultColor += 0.2 * lightColor;  // Ajoute une l�g�re lumi�re ambiante
    resultColor += spec * lightColor; // Ajoute la composante sp�culaire
    FragColor = vec4(resultColor, 1.0);
}