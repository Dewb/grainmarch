char vertexShaderCode[] = R"(

void main()
{    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}


)";

char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;
uniform float Intensity;

void main()
{
    vec2 texCoords = gl_TexCoord[0].st;
    gl_FragColor = texture2D(inputTexture0, texCoords);
    float d = 1.0 - 2.0 * abs(0.5 - gl_TexCoord[0].z);
    float dd = pow(d, 2.8);

    gl_FragColor.r += Intensity * 0.22 * dd;
    gl_FragColor.g += Intensity * 0.27 * dd;
    gl_FragColor.b += Intensity * 0.18 * dd;
    gl_FragColor.a = Intensity * pow(d, 2.5 - 2.0 * Intensity);
}

)";


