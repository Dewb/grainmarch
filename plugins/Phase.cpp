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

DECLARE_PLUGIN(SourcePlugin, "DBPH", "Phase", "Complex phase portrait experiment", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
PARAM(Domain, 0.1, 100.0, 1.2)
PARAM(w_x, -1.0, 1.0, 0.30902)
PARAM(w_y, -1.0, 1.0, 0.95106)
PARAM(t, pow(10, -8), pow(10, -4), pow(10, -8))
END_SHADER_PARAMETERS()


