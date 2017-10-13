char fragmentShaderCode[] = R"(

uniform sampler2D inputTexture0;

uniform vec2 texOffset;

varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform float Contrast;
uniform float Saturation;
uniform float Brightness;
uniform float Mix;
uniform float BlackBG;

// source: TGM's shader pack
// http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=21057
vec3 ContrastSaturationBrightness(vec3 color, float con, float sat, float brt)
{
    const float AvgLumR = 0.5;
    const float AvgLumG = 0.5;
    const float AvgLumB = 0.5;

    const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);

    vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
    vec3 brtColor = color * brt;
    vec3 intensity = vec3(dot(brtColor, LumCoeff));
    vec3 satColor = mix(intensity, brtColor, sat);
    vec3 conColor = mix(AvgLumin, satColor, con);
    
    return conColor;
}

vec3 rgb2hsb( in vec3 c ){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), 
                 vec4(c.gb, K.xy), 
                 step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), 
                 vec4(c.r, p.yzx), 
                 step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), 
                d / (q.x + e), 
                q.x);
}

//  Function from Iñigo Quiles 
//  https://www.shadertoy.com/view/MsS3Wc
vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0, 
                     0.0, 
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

void main() {
  gl_FragColor = texture2D(inputTexture0, gl_TexCoord[0]);
  gl_FragColor.rgb = ContrastSaturationBrightness(gl_FragColor.rgb, Contrast * 2.0, Saturation * 2.0, Brightness * 2.0);
  vec3 hsb = rgb2hsb(gl_FragColor.rgb);
  hsb.z = mix(hsb.z, hsb.y, Mix);
  gl_FragColor.rgb = hsb2rgb(hsb);
  //gl_FragColor.a = (BlackBG + hsb.z) % 1.0; 
}   




)";