#version 430
in vec4 v_coord;
in vec2 v_texcoord;
in vec3 v_normal;
uniform mat4 MVP;
uniform mat3 NM;

out vec4 color;
out vec2 texcoord;

void main()
{
    // pass texcoord to fragment shader (not used for the moment)
    texcoord = v_texcoord;

    // display normals transformed by the normal matrix NM
    vec3 N = normalize(NM * v_normal);
    color = vec4(abs(N), 1.0);

    // Transform vertex position
    gl_Position = MVP * v_coord;
}