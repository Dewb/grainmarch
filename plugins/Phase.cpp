//
//  Phase.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 10/27/14.
//
//

#include "PluginDefinition.h"
#include "Phase.glsl"
#include "default_vertex_shader.glsl"
#include <cmath>

#define PI 3.1415926535897932384626433832795

DECLARE_PLUGIN(SourcePlugin, "DBPH", "Phase", "Complex phase portrait experiment", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
PARAM(Zoom, 0.1, 40.0, 1.2)
PARAM(Direction, 0, 2 * PI, 0)
PARAM(Error, 0, 1.0, 0)
PARAM(ColorMode, 0, 1, 0);
PARAM(HueLimit, 0, 1, 1);
PARAM(HueOffset, 0, 1, 0);
PARAM(Saturation, 0, 1, 1);
PARAM(Function, 0, 1, 0);
PARAM(A, -100, 100.0, 1)
PARAM(B, -100, 100.0, -21)
PARAM(C, -100, 100.0, 35)
PARAM(D, -100, 100.0, -7)
PARAM(E, -100, 100.0, -6)
PARAM(F, -100, 100.0, 1)
PARAM(G, -100, 100.0, 1)
PARAM(H, -100, 100.0, -7)
PARAM(I, -100, 100.0, 35)
PARAM(J, -100, 100.0, 4)
PARAM(K, -100, 100.0, -21)
PARAM(L, -100, 100.0, -4)
PARAM(M, -100, 100.0, 1)
PARAM(N, -100, 100.0, 2)
END_SHADER_PARAMETERS()


