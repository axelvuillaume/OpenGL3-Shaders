in vec2 in_position;
out vec2 UV;
void main() {
// quad coordinates [-1,1] to tex coordinates [0,1]:
UV = (1.f+in_position.xy)/2.f;
// no MVP
gl_Position = vec4(in_position.x,in_position.y,0.0f,1.0f);
}

