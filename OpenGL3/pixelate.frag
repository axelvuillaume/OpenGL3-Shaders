#version 330 core

in vec2 UV;
uniform sampler2D renderedTexture;
uniform float pixelSize; // Contr�le la taille des pixels

out vec4 fColor;

void main()
{
    // Quantifier les coordonn�es UV en fonction de la taille de pixel d�sir�e
    vec2 pixelatedUV = floor(UV / pixelSize) * pixelSize;

    // Appliquer la texture sur les UV pixelis�es
    vec3 color = texture(renderedTexture, pixelatedUV).xyz;

    fColor = vec4(color, 1.0);
}
