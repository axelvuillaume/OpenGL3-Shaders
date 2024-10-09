#version 430

in vec3 P; // Position du fragment
in vec3 N; // Normale du fragment

uniform vec4 Ka; // Couleur ambiante
uniform vec4 Kd; // Couleur diffuse
uniform vec4 Ks; // Couleur spéculaire
uniform float shininess; // Exposant de brillance
uniform vec3 lightpos; // Position de la lumière
uniform vec3 lightColor; // Couleur de la lumière
uniform sampler2D tex; // Texture

out vec4 fColor; // Couleur finale du fragment

void main()
{
    // Normaliser la normale
    vec3 normalizedNormal = normalize(N);

    // Calculer le vecteur de la lumière et la direction de la vue
    vec3 L = normalize(lightpos - P); // Vecteur vers la lumière
    vec3 V = normalize(-P); // Vecteur vers la caméra (peut varier selon la caméra)

    // Calculer le vecteur réfléchi
    vec3 R = reflect(-L, normalizedNormal); 

    // Convertir lightColor en vec4
    vec4 lightColor4 = vec4(lightColor, 1.0);

    // Calculer les termes d'illumination
    // Couleur ambiante
    vec4 amb = Ka * lightColor4;

    // Couleur diffuse
    vec4 diff = Kd * lightColor4 * max(dot(normalizedNormal, L), 0.0);

    // Couleur spéculaire
    vec4 spec = Ks * lightColor4 * pow(max(dot(R, V), 0.0), shininess);

    // Appliquer la texture
    vec4 textureColor = texture(tex, gl_FragCoord.xy); // Appliquer la texture

    // Combiner les termes d'illumination avec la couleur de la texture
    fColor = (amb + diff + spec) * textureColor; // Multiplier par la couleur de la texture
    fColor = clamp(fColor, 0.0, 1.0); // S'assurer que la couleur reste entre 0 et 1
}

