#version 430

in vec2 UV;
out vec4 fColor;
uniform float time; // current time in seconds

void main() {
    // Calculer une couleur basée sur les coordonnées UV et le temps
    // Par exemple, un dégradé dynamique
    vec3 color = vec3(UV, 0.5 + 0.5 * sin(time)); // Change la composante verte en fonction du temps
    fColor = vec4(color, 1.0); // RGBA
}