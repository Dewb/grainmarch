//
//  SaturationFilter.cpp
//
//  Created by Michael Dewberry on 8/31/16.
//
//

#include "PluginDefinition.h"
#include "SaturationFilter.glsl"
#include "../default_vertex_shader.glsl"

DECLARE_PLUGIN(EffectPlugin, "DBSA", "SatFilter", "Filter desaturated areas to black/transparent", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
	PARAM(Contrast, 0.0, 1.0, 0.5)
	PARAM(Saturation, 0.0, 1.0, 0.5)
	PARAM(Brightness, 0.0, 1.0, 0.5)
    PARAM(Mix, 0.0, 1.0, 1.0)
	PARAM(BlackBG, 0.0, 1.0, 0.0, FF_TYPE_BOOLEAN)
END_SHADER_PARAMETERS()


