char vertexShaderCode[] = R"(

uniform float Columns;
uniform float Rows;

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
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

)";


