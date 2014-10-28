char fragmentShaderCode[] = R"(

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iGlobalTime;           // shader playback time (in seconds)
uniform sampler2D inputTexture0;

#define PI 3.1415926535897932384626433832795

uniform float Domain;
uniform float Direction;
uniform float Error;
uniform float ColorMode;
uniform float HueLimit;
uniform float HueOffset;
uniform float Saturation;
uniform float Function;

uniform float A; // 1
uniform float B; // 21
uniform float C; // 35
uniform float D; // 7
uniform float E; // 6
uniform float F; // 1
uniform float G; // 1

uniform float H; // -7
uniform float I; // 35
uniform float J; // 4
uniform float K; // 21
uniform float L; // 4
uniform float M; // 1
uniform float N; // 2


// From https://github.com/hughsk/glsl-hsv2rgb

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 f(vec2 z) {
    float x = z.x;
    float x2 = x * x;
    float x3 = x * x2;
    float x4 = x * x3;
    float x5 = x * x4;
    float x6 = x * x5;
    float x7 = x * x6;
    
    float y = z.y;
    float y2 = y * y;
    float y3 = y * y2;
    float y4 = y * y3;
    float y5 = y * y4;
    float y6 = y * y5;
    float y7 = y * y6;

    if (Function < 0.5) {
        return vec2(A * x7     + B * x5 * y2 + C * x3 * y4 + D * x * y7  + E * x2 * y2 + F * y4 + G,
                    H * x6 * y + I * x4 * y3 + J * x3 * y  + K * x2 * y5 + L * x * y3  + M * y7 + N * y) +
         vec2(x * x + x * y, x * x - y * y);
    } else {
        return vec2(x * x + x * y, x * x - y * y);
    }
}

vec4 phasePortraitColor(float phase) {
    vec4 color;
    float phaseScaled = phase / (2.0 * PI);
    float hue = mod(phaseScaled * HueLimit + HueOffset, 1.0);
    float v = 1.0 - (1.0 - Saturation) * phaseScaled;
    color.rgb = hsv2rgb(vec3(hue, Saturation, v));
    color.a = 1.0;
    return color;
}

void main(void)
{
    float d = mod(Direction + PI/4.0, 2.0 * PI);
    vec2 w = vec2(cos(d), sin(d));
    float t = Domain * pow(10.0, -Error * 9.0 - 2.0);
    
    vec2 z0 = (gl_TexCoord[0].st - 0.5) * 2.0 * Domain;
	vec2 fz0 = f(z0);
    vec2 fzd = f(z0 + t * w);
    vec2 fk = (fzd - fz0) / (t * w);
    
    float phase = atan(fk.y, fk.x);
    if (phase < 0.0) {
        phase += 2.0 * PI;
    }
    
    if ((d >= PI/2.0 && d < PI)|| (d >= 3.0*PI/2.0)) {
        phase = mod(phase + PI, 2.0 * PI);
        fk = -fk;
    }
    
    if (ColorMode < 0.25) {
        gl_FragColor = phasePortraitColor(phase);
    } else if (ColorMode < 0.5) {
        gl_FragColor = vec4(fk.x, fk.y, 0.0, 1.0);
    } else if (ColorMode < 0.75) {
        gl_FragColor = vec4(fk.x, 0, fk.y, 1.0);
    } else {
        gl_FragColor = vec4(0, fk.x, fk.y, 1.0);
    }
}


)";