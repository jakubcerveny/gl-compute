#line 2
#if _VERTEX_

in vec3 position;
out vec4 vertColor;

void main()
{
    int n = gl_VertexID / 3;
    vertColor = vec4(RGB_Palette_3[n * 4], 1);
    gl_Position = vec4(position, 1);
}


#elif _FRAGMENT_

in vec4 vertColor;
out vec4 fragColor;

void main()
{
    fragColor = vertColor;
}

#endif
