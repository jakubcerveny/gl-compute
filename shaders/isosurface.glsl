#line 2

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding = 0) buffer triangle_buffer
{
    vec4 vertices[];
};

layout(std430, binding = 1) buffer counter_buffer
{
    uint num_triangles;
};

layout(std430, binding = 2) buffer tables_buffer
{
    int edgeTable[256];
    int triTable[256][16];
};


void main()
{
    uint i = atomicAdd(num_triangles, 1);

    vertices[3*i + 0] = vec4( 0.0, triTable[1][2], 0.0, 1);
    vertices[3*i + 1] = vec4( 0.5, -0.5, 0.0, 1);
    vertices[3*i + 2] = vec4(-0.5, -0.5, 0.0, 1);
}

