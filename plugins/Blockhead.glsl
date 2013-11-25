char vertexShaderCode[] = R"(

uniform float Columns;
uniform float Rows;
uniform float Tower1Pos;
uniform float Tower1Height;
uniform float Tower1Width;
uniform float Tower2Pos;
uniform float Tower2Height;
uniform float Tower2Width;
uniform float Tower3Pos;
uniform float Tower3Height;
uniform float Tower3Width;
uniform float Tower4Pos;
uniform float Tower4Height;
uniform float Tower4Width;

bool cubeIsVisible()
{
    float x = gl_MultiTexCoord1[0] / Columns;
    float dx = 1.0 / (Columns * 2.0);
    float y = gl_MultiTexCoord1[1] / Rows;
    if (abs(Tower1Pos - x) < dx * Tower1Width && y < Tower1Height)
        return true;
    if (abs(Tower2Pos - x) < dx * Tower2Width && y < Tower2Height)
        return true;
    if (abs(Tower3Pos - x) < dx * Tower3Width && y < Tower3Height)
        return true;
    if (abs(Tower3Pos - x) < dx * Tower4Width && y < Tower4Height)
        return true;
    return false;
}

void main()
{
    //if (!cubeIsVisible) discard;
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}


)";

char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;

void main()
{
    vec2 texCoords = gl_TexCoord[0].st;
    //if (texCoords.s < 0.05 || texCoords.s > 0.95 || texCoords.t < 0.05 || texCoords.t > 0.95)
    //    texCoords = vec2(0.0, 0.0);
    gl_FragColor = texture2D(inputTexture0, texCoords);
}

)";


