//
//  Tanline.cpp
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include "Average.glsl"

DECLARE_PLUGIN(EffectPlugin, "DBAV", "Average", "Average hue using hardware mipmap", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM(Level, 0.0, 16.0, 4.0)
END_SHADER_PARAMETERS()


