char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;
uniform float Level;

void main(void)
{
	gl_FragColor = texture2D(inputTexture0, gl_TexCoord[0].st);
}


)";