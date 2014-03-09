char vertexShaderCode[] = R"(

void main()
{    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}


)";

char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;

void main()
{
    vec2 texCoords = gl_TexCoord[0].st;
    gl_FragColor = texture2D(inputTexture0, texCoords);
    gl_FragColor.a = 1.0;
}

)";


