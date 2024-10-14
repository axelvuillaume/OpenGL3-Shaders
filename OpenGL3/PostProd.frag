#version 330 core

in vec2 UV;
out vec4 fColor; // Couleur finale du fragment

uniform sampler2D renderedTexture;
uniform float SIZEX; // Largeur de la texture
uniform float SIZEY; // Hauteur de la texture

void main() {
    // Récupération des pixels voisins dans 8 directions autour du pixel courant
    vec4 top = texture(renderedTexture, vec2(UV.x, UV.y + 1.0 / SIZEY));
    vec4 bottom = texture(renderedTexture, vec2(UV.x, UV.y - 1.0 / SIZEY));
    vec4 left = texture(renderedTexture, vec2(UV.x - 1.0 / SIZEX, UV.y));
    vec4 right = texture(renderedTexture, vec2(UV.x + 1.0 / SIZEX, UV.y));
    vec4 topLeft = texture(renderedTexture, vec2(UV.x - 1.0 / SIZEX, UV.y + 1.0 / SIZEY));
    vec4 topRight = texture(renderedTexture, vec2(UV.x + 1.0 / SIZEX, UV.y + 1.0 / SIZEY));
    vec4 bottomLeft = texture(renderedTexture, vec2(UV.x - 1.0 / SIZEX, UV.y - 1.0 / SIZEY));
    vec4 bottomRight = texture(renderedTexture, vec2(UV.x + 1.0 / SIZEX, UV.y - 1.0 / SIZEY));

    // Calcul des dérivées de Sobel sur les axes X et Y
    vec4 sx = -topLeft - 2.0 * left - bottomLeft + topRight + 2.0 * right + bottomRight;
    vec4 sy = -topLeft - 2.0 * top - topRight + bottomLeft + 2.0 * bottom + bottomRight;

    // Magnitude combinée de Sobel
    vec4 sobel = sqrt(sx * sx + sy * sy);

    // Couleur finale des fragments
    fColor = sobel;
}