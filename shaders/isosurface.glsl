#line 2

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding = 0) buffer vertex_buffer
{
    vec4 vertices[];
};

layout(std430, binding = 1) buffer counter_buffer
{
    uint total_vertices;
};

layout(std430, binding = 2) buffer tables_buffer
{
    int edge_table[256];
    int tri_table[256][16];
};

uniform float voxel_size;
uniform float iso_value;

const uvec3 corner_ijk[8] =
{
    uvec3(0, 0, 0),
    uvec3(1, 0, 0),
    uvec3(1, 1, 0),
    uvec3(0, 1, 0),
    uvec3(0, 0, 1),
    uvec3(1, 0, 1),
    uvec3(1, 1, 1),
    uvec3(0, 1, 1)
};

vec3 grid_point(vec3 ijk)
{
    return voxel_size * ijk + vec3(-1, -1, -1);
}

const float eps = 1e-5;

vec3 get_vertex(float iso, vec3 p1, vec3 p2, float val1, float val2)
{
    if (abs(iso - val1) < eps) { return p1; }
    if (abs(iso - val2) < eps) { return p2; }
    if (abs(val2 - val1) < eps) { return p1; }

    return p1 + (iso - val1) / (val2 - val1) * (p2 - p1);
}

float implicit_function(vec3 x)
{
    return length(x);
}


void main()
{
    vec3 corner[8];
    float value[8];
    vec3 vertex[12];

    uint cube_index = 0;
    for (uint i = 0, bit = 1; i < 8; i++, bit *= 2)
    {
        corner[i] = grid_point(gl_GlobalInvocationID + corner_ijk[i]);
        value[i] = implicit_function(corner[i]);

        if (value[i] < iso_value) {
            cube_index |= bit;
        }
    }

    int edge_mask = edge_table[cube_index];
    if (edge_mask == 0) {
        // voxel completely in/out of the surface
        return;
    }

    if ((edge_mask & 1) != 0) {
       vertex[0] =
          get_vertex(iso_value, corner[0], corner[1], value[0], value[1]);
    }
    if ((edge_mask & 2) != 0) {
       vertex[1] =
          get_vertex(iso_value, corner[1], corner[2], value[1], value[2]);
    }
    if ((edge_mask & 4) != 0) {
       vertex[2] =
          get_vertex(iso_value, corner[2], corner[3], value[2], value[3]);
    }
    if ((edge_mask & 8) != 0) {
       vertex[3] =
          get_vertex(iso_value, corner[3], corner[0], value[3], value[0]);
    }
    if ((edge_mask & 16) != 0) {
       vertex[4] =
          get_vertex(iso_value, corner[4], corner[5], value[4], value[5]);
    }
    if ((edge_mask & 32) != 0) {
       vertex[5] =
          get_vertex(iso_value, corner[5], corner[6], value[5], value[6]);
    }
    if ((edge_mask & 64) != 0) {
       vertex[6] =
          get_vertex(iso_value, corner[6], corner[7], value[6], value[7]);
    }
    if ((edge_mask & 128) != 0) {
       vertex[7] =
          get_vertex(iso_value, corner[7], corner[4], value[7], value[4]);
    }
    if ((edge_mask & 256) != 0) {
       vertex[8] =
          get_vertex(iso_value, corner[0], corner[4], value[0], value[4]);
    }
    if ((edge_mask & 512) != 0) {
       vertex[9] =
          get_vertex(iso_value, corner[1], corner[5], value[1], value[5]);
    }
    if ((edge_mask & 1024) != 0) {
       vertex[10] =
          get_vertex(iso_value, corner[2], corner[6], value[2], value[6]);
    }
    if ((edge_mask & 2048) != 0) {
       vertex[11] =
          get_vertex(iso_value, corner[3], corner[7], value[3], value[7]);
    }

    uint nv = tri_table[cube_index][15];
    uint pos = atomicAdd(total_vertices, nv);

    for (uint i = 0; i < nv; i++) {
        vertices[pos++] = vec4(vertex[tri_table[cube_index][i]], 1);
    }
}

