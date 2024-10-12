in vec2 UV;
uniform sampler2D renderedTexture;
out vec4 fColor;
void main() {
fColor = vec4(texture(renderedTexture, UV).xyz, 1.0);
}