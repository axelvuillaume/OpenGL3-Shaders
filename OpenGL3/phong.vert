#version 430
in vec4 v_coord;
in vec3 v_normal;
uniform mat4 MV; // ModelView
uniform mat4 MVP; // Projection * ModelView
uniform mat3 NM; // Normal Matrix
out vec3 P;
out vec3 N;
void main()
{
// compute P, N:
P = vec3(MV * v_coord);
N = normalize(NM * v_normal);
gl_Position = MVP*v_coord;
}