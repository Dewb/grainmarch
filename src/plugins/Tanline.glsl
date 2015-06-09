char fragmentShaderCode[] = R"(

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iGlobalTime;           // shader playback time (in seconds)
uniform sampler2D inputTexture0;

uniform float StripeHeight;
uniform float NotchHeight;
uniform float Advance;
uniform float SlideStripes;
uniform float RepeatY;
uniform float RepeatX;
uniform float StretchX;
uniform float OffsetY;
uniform float OffsetX;
uniform float Mix;

void main(void)
{
	float y = mod(gl_TexCoord[0].t + OffsetY, 1.0);
	float x = mod(gl_TexCoord[0].s + OffsetX, 1.0);
	
	float band = floor((1.0 - y) / StripeHeight);
	float m = mod((1.0 - y), StripeHeight);
	if (m / StripeHeight < NotchHeight && x < mod(Advance * band, 1.0)) {
		band -= 1.0;
	}
	
	vec2 nudge = vec2(0.0, 0.0);
	if (RepeatY > 0.0) {
		nudge -= mod(y, RepeatY * 0.05);
	}
	if (RepeatX > 0.0) {
		nudge += mod(x, RepeatX * 0.1);
	}
    
    float xprime = 0.5 + (1.0 - StretchX) * (x - 0.5);
    float yprime = 1.0 - band * StripeHeight;

    if (SlideStripes > 0.0) {
        xprime += band * Advance;
    }
    
	vec2 fxCoord = vec2(xprime, yprime) + nudge;
    vec4 fxColor = texture2D(inputTexture0, fxCoord);

    vec2 origCoord = vec2(xprime, y);
    vec4 origColor = texture2D(inputTexture0, origCoord);
    
	gl_FragColor = mix(fxColor, origColor, Mix);
}


)";