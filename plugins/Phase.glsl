char fragmentShaderCode[] = R"(

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iGlobalTime;           // shader playback time (in seconds)
uniform sampler2D inputTexture0;

uniform float Domain;
uniform float w_x;
uniform float w_y;
uniform float t;

vec2 f(vec2 z) {
    float x = z.x;
    float y = z.y;
    return vec2(x * x + x * y, x * x - y * y);
}

void main(void)
{
    vec2 w = vec2(w_x, w_y);
    vec2 z0 = (gl_TexCoord[0].st - 0.5) * 2.0 * Domain;
	vec2 fz0 = f(z0);
    vec2 fzd = f(z0 + t * w);
    vec2 fk = (fzd - fz0) / (t * w);
    
	gl_FragColor = vec4(fk.x, fk.y, 0.0, 1.0);
}


)";