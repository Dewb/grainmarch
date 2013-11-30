char vertexShaderCode[] = R"(

uniform sampler2D inputTexture0;
uniform float Level;
varying vec4 color;

void main()
{    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    color = texture2D(inputTexture0, gl_TexCoord[0].st);
}


)";

char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;
uniform float Level;
varying vec4 color;

void main(void)
{
	gl_FragColor = color;
}


)";