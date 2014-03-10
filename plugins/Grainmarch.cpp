//
//  Grainmarch.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include <cmath>

#include "Grainmarch.glsl"
#include "default_vertex_shader.glsl"

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

float g_totalTime = 0.0;

auto timeAction = [](Parameter& p, float v, ParamList& l) {
    static float currentTime = 0.0;
    static float totalTicks = 0.0;
    
    if (currentTime - v > 0.5) {
        totalTicks += 1.0;
    } else if (v - currentTime > 0.5) {
        totalTicks -= 1.0;
    }
    currentTime = v;
    g_totalTime = totalTicks + currentTime;
};

BEGIN_SHADER_PARAMETERS()
    PARAM(Time, 0.0, 10.0, 0.0, FF_TYPE_STANDARD, false, false, timeAction)
    PARAM(SymmetryMode, 0.0, 1.0, 0.0)
    PARAM(FieldOfView, 0.1, 10.0, 1.0)
    PARAM(Iterations, 2, 20.0, 5)
    PARAM(Scale, 1.0, 10.0, 3.0)
    PARAM(ZoomSpeed, -2.0, 2.0, 0.5)
    PARAM(NonLinearPerspective, -10.0, 10.0, 2.0)
    PARAM(NLPOnly, 0.0, 1.0, 0.0)
    PARAM(OffsetX, 0.0, 1.0, 0.5)
    PARAM(OffsetY, 0.0, 1.0, 0.5)
    PARAM(OffsetZ, 0.0, 1.0, 0.5)
    PARAM(Color1R, 0.0, 1.0, 1.0)
    PARAM(Color1G, 0.0, 1.0, 1.0)
    PARAM(Color1B, 0.0, 1.0, 0.858824)
    PARAM(Color2R, 0.0, 1.0, 0.0)
    PARAM(Color2G, 0.0, 1.0, 0.333333)
    PARAM(Color2B, 0.0, 1.0, 1.0)
    PARAM(Ambient, 0.0, 1.0, 0.32184)
    PARAM(Diffuse, 0.0, 1.0, 0.7)
    PARAM(Jitter, 0.0, 0.2, 0.05)
    PARAM(IterationRotation, -10.0, 10.0, 4.0)
    PARAM(IterationRotationLFO, -1.0, 1.0, 0.125)
    PARAM(IterationRotationLFOIntensity, 0.0, 10.0, 2.0)
    PARAM(NLPRotationLFO, -1.0, 1.0, 0.25)
END_SHADER_PARAMETERS()

class GrainmarchPlugin : public SourcePlugin
{
public:
    GrainmarchPlugin() {}
    
    void Initialize() {
        ManuallyBindUniform("Time", &g_totalTime);
    }
};

DECLARE_PLUGIN(GrainmarchPlugin, "DBGM", "Grainmarch", "Grainy fractal raymarching", "by Michael Dewberry - dewb.org\nBased on Shadertoy shaders by Syntopia and Kali")


