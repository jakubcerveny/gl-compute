#line 2

#if _VERTEX_

const vec4 vertices[4] = {
    { -1, -1, 0, 1 },
    {  1, -1, 0, 1 },
    {  1,  1, 0, 1 },
    { -1,  1, 0, 1 }
};

const vec2 texCoords[4] = {
    { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 }
};

out vec2 texPos;

void main()
{
    gl_Position = vertices[gl_VertexID];
    texPos = texCoords[gl_VertexID];
}


#elif _FRAGMENT_

in vec2 texPos;

uniform sampler2D sampler;

void main()
{
    //gl_FragColor = vec4(texPos.x, texPos.y, 0, 1);
    gl_FragColor = texture(sampler, texPos);
}

#endif

