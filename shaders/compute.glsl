#line 2

layout (local_size_x = 4, local_size_y = 4) in;

layout (rgba32f, binding = 0) uniform image2D img_out;

void main()
{
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    //vec4 color = vec4(0, 0, 1, 1);

    /*vec4 color = vec4(float(gl_WorkGroupID.x) / gl_NumWorkGroups.x,
                      float(gl_WorkGroupID.y) / gl_NumWorkGroups.y,
                      0, 1);*/

    vec4 color = vec4(float(gl_LocalInvocationID.x) / gl_WorkGroupSize.x,
                      float(gl_LocalInvocationID.y) / gl_WorkGroupSize.y,
                      0, 1);

    imageStore(img_out, pixel_coords, color);
}

