char vertexShaderCode[] = R"(

void main()
{    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
}


)";

char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;
uniform float Border;

void main()
{
    vec2 texCoords = gl_TexCoord[0].st;
    vec2 edgeCoords = gl_TexCoord[1].st;
    if (edgeCoords.s < Border || edgeCoords.s > (1.0 - Border) || edgeCoords.t < Border || edgeCoords.t > (1.0 - Border))
        texCoords = vec2(0.0, 0.0);
    gl_FragColor = texture2D(inputTexture0, texCoords);
}

)";


