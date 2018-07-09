#line 2

#if _VERTEX_

in vec2 position;

const vec4 vertices[4] = {
    { -1.0, -1.0, 0.0, 1.0 },
    {  1.0, -1.0, 0.0, 1.0 },
    {  1.0,  1.0, 0.0, 1.0 },
    { -1.0,  1.0, 0.0, 1.0 }
};

void main()
{
    gl_Position = vertices[gl_VertexID];
}


#elif _FRAGMENT_

void main()
{
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif

