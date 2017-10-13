//
//  Grainmarch.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include <cmath>

#include "Menger.glsl"
#include "../default_vertex_shader.glsl"


DECLARE_PLUGIN(SourcePlugin, "DBMG", "Menger", "Menger Sponge fractal raymarching", "by Michael Dewberry - dewb.org\nBased on Shadertoy shaders by Syntopia and Kali")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM(Time, 0.0, 10.0, 0.0, FF_TYPE_STANDARD, true, true, AccumulateAction)
    PARAM(SymmetryMode, 0.0, 1.0, 0.0)
    PARAM(FieldOfView, 0.1, 10.0, 1.0)
    PARAM(Iterations, 2, 20.0, 5)
    PARAM(Scale, 1.0, 10.0, 3.0)
    PARAM(ZoomSpeed, -2.0, 2.0, 0.5)
    PARAM(NonLinearPerspective, -10.0, 10.0, 2.0)
    PARAM(NLPOnly, 0.0, 1.0, 0.0, FF_TYPE_BOOLEAN)
    PARAM(OffsetX, 0.0, 1.0, 0.5)
    PARAM(OffsetY, 0.0, 1.0, 0.5)
    PARAM(OffsetZ, 0.0, 1.0, 0.5)
    PARAM(Color1, 0.0, 1.0, 1.0, FF_TYPE_HUE)
    PARAM(Color1S, 0.0, 1.0, 1.0, FF_TYPE_SATURATION)
    PARAM(Color1B, 0.0, 1.0, 0.858824, FF_TYPE_BRIGHTNESS)
	PARAM(Color2, 0.0, 1.0, 1.0, FF_TYPE_HUE)
	PARAM(Color2S, 0.0, 1.0, 1.0, FF_TYPE_SATURATION)
	PARAM(Color2B, 0.0, 1.0, 0.858824, FF_TYPE_BRIGHTNESS)
	PARAM(Ambient, 0.0, 1.0, 0.32184)
    PARAM(Diffuse, 0.0, 1.0, 0.7)
    PARAM(Jitter, 0.0, 0.2, 0.05)
    PARAM(IterationRotation, -10.0, 10.0, 4.0)
    PARAM(IterationRotationLFO, -1.0, 1.0, 0.125)
    PARAM(IterationRotationLFOIntensity, 0.0, 10.0, 2.0)
    PARAM(NLPRotationLFO, -1.0, 1.0, 0.25)
END_SHADER_PARAMETERS()


