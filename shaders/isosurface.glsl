#line 2

layout(local_size_x = 1, local_size_y = 1) in;
//layout(local_size_variable) in;

layout(std430, binding = 3) buffer buf
{
    vec4 vertices[];
};


void main()
{
    int i = int(gl_GlobalInvocationID.x);

    vertices[0] = vec4( 0.0,  0.5, 0.0, 1);
    vertices[1] = vec4( 0.5, -0.5, 0.0, 1);
    vertices[2] = vec4(-0.2, -0.5, 0.0, 1);
}

