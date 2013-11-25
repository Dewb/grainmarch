char fragmentShaderCode[] = R"(

// GLSL code from Menger Journey by Syntopia
// https://shadertoy.com/view/Mdf3z7

uniform float      iResolutionX;           // viewport resolution (in pixels)
uniform float      iResolutionY;           // viewport resolution (in pixels)
uniform float     iGlobalTime;           // shader playback time (in seconds)
//uniform float     iChannelTime[4];       // channel playback time (in seconds)
//uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
//uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
//uniform samplerXX iChannel0..3;          // input channel. XX = 2D/Cube
//uniform vec4      iDate;                 // (year, month, day, time in seconds)

uniform float FieldOfView;
uniform float Iterations;
uniform float Scale;
uniform float Speed;
uniform float IterationRotation;
uniform float IterationRotationLFO;
uniform float IterationRotationLFOIntensity;
uniform float NLPRotationLFO;

uniform float NonLinearPerspective;
uniform float NLPOnly;

uniform float OffsetX;
uniform float OffsetY;
uniform float OffsetZ;

uniform float Color1R;
uniform float Color1G;
uniform float Color1B;
uniform float Color2R;
uniform float Color2G;
uniform float Color2B;

uniform float Ambient;
uniform float Diffuse;
uniform float Jitter;

//#define FieldOfView 1.0
//#define Iterations 7
//#define Scale 3.0
//#define Speed 1.0

#define MaxSteps 30
#define MinimumDistance 0.0009
#define normalDistance     0.0002

#define PI 3.141592
//#define Jitter 0.05
#define FudgeFactor 0.7
//#define NonLinearPerspective 2.0
//#define DebugNonlinearPerspective true

//#define Ambient 0.32184
//#define Diffuse 0.7
#define LightDir vec3(1.0)
#define LightColor vec3(Color1R, Color1G, Color1B)
#define LightDir2 vec3(1.0,-1.0,1.0)
#define LightColor2 vec3(Color2R, Color2G, Color2B)

vec3 Offset = vec3(0.92858 + (OffsetX - 0.5) * 1.0,
				   0.92858 + (OffsetY - 0.5) * 1.0,
				   0.32858 + (OffsetZ - 0.5) * 1.0);

vec2 rotate(vec2 v, float a) {
	return vec2(cos(a)*v.x + sin(a)*v.y, -sin(a)*v.x + cos(a)*v.y);
}

// Two light sources. No specular 
vec3 getLight(in vec3 color, in vec3 normal, in vec3 dir) {
	vec3 lightDir = normalize(LightDir);
	float diffuse = max(0.0,dot(-normal, lightDir)); // Lambertian
	
	vec3 lightDir2 = normalize(LightDir2);
	float diffuse2 = max(0.0,dot(-normal, lightDir2)); // Lambertian
	
	return
	(diffuse*Diffuse)*(LightColor*color) +
	(diffuse2*Diffuse)*(LightColor2*color);
}


// DE: Infinitely tiled Menger IFS.
//
// For more info on KIFS, see:
// http://www.fractalforums.com/3d-fractal-generation/kaleidoscopic-%28escape-time-ifs%29/
float DE(in vec3 z)
{
	// enable this to debug the non-linear perspective
	if (NLPOnly == 1.0) {
		z = fract(z);
		float d=length(z.xy-vec2(0.5));
		d = min(d, length(z.xz-vec2(0.5)));
		d = min(d, length(z.yz-vec2(0.5)));
		return d-0.01;
	}
	// Folding 'tiling' of 3D space;
	z  = abs(1.0-mod(z,2.0));

	float d = 1000.0;
	for (int n = 0; n < int(Iterations); n++) {
		z.xy = rotate(z.xy, IterationRotation + IterationRotationLFOIntensity * cos(iGlobalTime * IterationRotationLFO));		
		z = abs(z);
		if (z.x<z.y){ z.xy = z.yx;}
		if (z.x< z.z){ z.xz = z.zx;}
		if (z.y<z.z){ z.yz = z.zy;}
		z = Scale*z-Offset*(Scale-1.0);
		if( z.z<-0.5*Offset.z*(Scale-1.0))  z.z+=Offset.z*(Scale-1.0);
		d = min(d, length(z) * pow(Scale, float(-n)-1.0));
	}
	
	return d-0.001;
}

// Finite difference normal
vec3 getNormal(in vec3 pos) {
	vec3 e = vec3(0.0,normalDistance,0.0);
	
	return normalize(vec3(
			DE(pos+e.yxx)-DE(pos-e.yxx),
			DE(pos+e.xyx)-DE(pos-e.xyx),
			DE(pos+e.xxy)-DE(pos-e.xxy)
			)
		);
}

// Solid color 
vec3 getColor(vec3 normal, vec3 pos) {
	return vec3(1.0);
}


// Pseudo-random number
// From: lumina.sourceforge.net/Tutorials/Noise.html
float rand(vec2 co){
	return fract(cos(dot(co,vec2(4.898,7.23))) * 23421.631);
}

vec4 rayMarch(in vec3 from, in vec3 dir) {
	// Add some noise to prevent banding
	float totalDistance = Jitter*rand(gl_FragCoord.xy+vec2(iGlobalTime));
	vec3 dir2 = dir;
	float distance;
	int steps = 0;
	vec3 pos;
	for (int i=0; i < MaxSteps; i++) {
		// Non-linear perspective applied here.
		dir.zy = rotate(dir2.zy, totalDistance * cos(iGlobalTime * NLPRotationLFO) * NonLinearPerspective);
		
		pos = from + totalDistance * dir;
		distance = DE(pos)*FudgeFactor;
		totalDistance += distance;
		if (distance < MinimumDistance) break;
		steps = i;
	}
	
	// 'AO' is based on number of steps.
	// Try to smooth the count, to combat banding.
	float smoothStep =   float(steps) + distance/MinimumDistance;
	float ao = 1.1-smoothStep/float(MaxSteps);
	
	// Since our distance field is not signed,
	// backstep when calc'ing normal
	vec3 normal = getNormal(pos-dir*normalDistance*3.0);
	
	vec3 color = getColor(normal, pos);
	vec3 light = getLight(color, normal, dir);
	color = (color*Ambient+light)*ao;
	return vec4(color,1.0);
}

void main(void)
{
	// Camera position (eye), and camera target
	vec3 camPos = Speed * iGlobalTime * vec3(1.0,0.0,0.0);
	vec3 target = camPos + vec3(1.0,0.0*cos(iGlobalTime),0.0*sin(0.4*iGlobalTime));
	vec3 camUp  = vec3(0.0,1.0,0.0);
	
	// Calculate orthonormal camera reference system
	vec3 camDir   = normalize(target-camPos); // direction for center ray
	camUp = normalize(camUp-dot(camDir,camUp)*camDir); // orthogonalize
	vec3 camRight = normalize(cross(camDir,camUp));
	
    vec2 res = vec2(iResolutionX, iResolutionY);
    
	vec2 coord =-1.0+2.0*gl_FragCoord.xy/res.xy;
	coord.x *= res.x/res.y;
	
	// Get direction for this pixel
	vec3 rayDir = normalize(camDir + (coord.x*camRight + coord.y*camUp)*FieldOfView);
	
	gl_FragColor = rayMarch(camPos, rayDir);
}





)";