#line 2
#if _VERTEX_

in vec4 position;
//out vec4 vertColor;
uniform mat4 MVP;

void main()
{
    //int n = gl_VertexID / 3;
    //vertColor = vec4(RGB_Palette_3[n * 4], 1);
    gl_Position = MVP * position;
}


#elif _FRAGMENT_

//in vec4 vertColor;
out vec4 fragColor;

void main()
{
    //fragColor = vertColor;
    fragColor = vec4(0, 0, 0, 1);
}

#endif
