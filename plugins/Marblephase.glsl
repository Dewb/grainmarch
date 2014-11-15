char fragmentShaderCode[] = R"(

#version 120

uniform float     iGlobalTime;
uniform sampler2D inputTexture0;
uniform int iResolutionX;
uniform int iResolutionY;

#define PI 3.1415926535897932384626433832795

uniform float Zoom;
uniform float Direction;
uniform float Imprecision;

uniform float Rotation;
uniform float Stripes;
uniform float LogStripePeriod;

uniform float ColorMode;
uniform float HueLimit;
uniform float HueShift;
uniform float Saturation;
uniform float Overexpose;
uniform float LogOverexpose;

uniform float ShiftX;
uniform float ShiftY;

uniform float K[32];

// hsv<->rgb routines from http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// sigmoid function from https://github.com/zzorn/pipedream/blob/master/assets/shaders/ShaderUtils.glsl

float sigmoid(float x) {
    if (x >= 1.0) return 1.0;
    else if (x <= -1.0) return 0.0;
    else return 0.5 + x * (1.0 - abs(x) * 0.5);
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
   
    float real =
        K[0] * x * y7 +
        K[1] * x6 * y2 +
        K[2] * x7 +
        K[3] * x5 * y2 +
        K[4] * x3 * y3 +
        K[5] * x * y5 +
        K[6] * x5 +
        K[7] * y5 +
        K[8] * x2 * y2 +
        K[9] * y4 +
        K[10] * x3 +
        K[11] * x2 * y +
        K[12] * x * y +
        K[13] * y2 +
        K[14] * x +
        K[15];
    
    float imag =
        K[16] * x6 * y +
        K[17] * y7 +
        K[18] * x4 * y3 +
        K[19] * x2 * y5 +
        K[20] * x3 * y3 +
        K[21] * x5 * y +
        K[22] * x2 * y3 +
        K[23] * y5 +
        K[24] * x3 * y +
        K[25] * x * y3 +
        K[26] * x2 * y +
        K[27] * y3 +
        K[28] * x2 +
        K[29] * y2 +
        K[30] * y +
        K[31];

    return vec2(real, imag);

}

float stripeRadius(float radius) {
    return Stripes * mod(log(radius), LogStripePeriod) / LogStripePeriod;
}

vec3 phasePortraitColor1(float phase, float r) {
    float phaseScaled = phase / (2.0 * PI);
    float hue = mod(phaseScaled * HueLimit + HueShift, 1.0);
    float v = 1.0 + Overexpose / 5.0 - (1.0 - Saturation) * phaseScaled - stripeRadius(r);
    return hsv2rgb(vec3(hue, Saturation, v));
}

vec3 phasePortraitColor2(float phase, float r) {
    float phaseScaled = phase / (2.0 * PI);
    float hue = mod((1.0 - 2.0 * abs(phaseScaled - 0.5)) * HueLimit + HueShift, 1.0);
    float v = 1.0 + Overexpose / 5.0 - (1.0 - Saturation) * phaseScaled - stripeRadius(r);
    return hsv2rgb(vec3(hue, Saturation, v));
}

vec2 rotate(vec2 z) {
    float s = sin(Rotation);
    float c = cos(Rotation);
    float xnew = z.x * c - z.y * s;
    float ynew = z.x * s + z.y * c;
    return vec2(xnew, ynew);
}

void main(void)
{
    float domain = 0.06 + sigmoid(Zoom - 1.0) * 25.0;
    
    float d = mod(Direction + PI/4.0, 2.0 * PI);
    vec2 w = vec2(cos(d), sin(d));
    float t = domain * pow(10.0, -Imprecision * 9.0 - 2.0);
    
    vec2 tex = gl_TexCoord[0].st;
    
    vec2 z0 = rotate(tex + vec2(2.0 * ShiftX - 1.0, 2.0 * ShiftY - 1.0) - vec2(0.5, 0.5)) * 2.0 * domain;
	vec2 fz0 = f(z0);
    vec2 fzd = f(z0 + t * w);
    vec2 fk = (fzd - fz0) / (t * w);
    
    float phase = atan(fk.y, fk.x);
    float radius = sqrt(fk.x * fk.x + fk.y * fk.y);
    if (phase < 0.0) {
        phase += 2.0 * PI;
    }
    
    if ((d >= PI/2.0 && d < PI)|| (d >= 3.0*PI/2.0)) {
        phase = mod(phase + PI, 2.0 * PI);
        fk = -fk;
    }
    
    if (ColorMode < 0.2) {
        gl_FragColor.rgb = phasePortraitColor1(phase, radius);
    } else if (ColorMode < 0.4) {
        gl_FragColor.rgb = phasePortraitColor2(phase, radius);
    } else {
        float c1 = fk.x;
        float c2 = fk.y;
        float c3 = log(max(c1, c2));
        float c4 = 1.0 - min(c1, c2);
        vec3 color;
        vec3 ntscWeights = vec3(0.299, 0.587, 0.114);
        if (ColorMode < 0.6) {
            color = vec3(c1, c2, 0);
        } else if (ColorMode < 0.8) {
            color = vec3(c3, c1, c2);
        } else {
            color = vec3(c1, c4, c2);
            ntscWeights = vec3(0.1, 0.487, 0.0);
        }
        vec3 hsv = rgb2hsv(color);
        float h = hsv.x;
        hsv.x = mod(h * HueLimit + HueShift, 1.0);
        if (hsv.z > LogOverexpose) { hsv.z = LogOverexpose; }
        hsv.z *= 1.0 - stripeRadius(radius);
        color = hsv2rgb(hsv);
        
        float gray = dot(color.rgb, ntscWeights);
        vec3 grayVec = vec3(gray, gray, gray);
        gl_FragColor.rgb = mix(grayVec, color, sigmoid(Saturation - 1.0) * 2.0);
        
    }
    gl_FragColor.a = 1.0;
}


)";