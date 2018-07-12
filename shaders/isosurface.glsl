#line 2

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding = 3) buffer triangle_buffer
{
    vec4 vertices[];
};

layout(std430, binding = 4) buffer atomic_buffer
{
    uint num_triangles;
};


void main()
{
    uint i = atomicAdd(num_triangles, 1);

    vertices[3*i + 0] = vec4( 0.0,  0.5, 0.0, 1);
    vertices[3*i + 1] = vec4( 0.5, -0.5, 0.0, 1);
    vertices[3*i + 2] = vec4(-0.5, -0.5, 0.0, 1);
}

