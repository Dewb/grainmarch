//
//  Tanline.cpp
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include "Tanline.glsl"
#include "default_vertex_shader.glsl"

DECLARE_PLUGIN(EffectPlugin, "DBTN", "Tanline", "Platonic scanline glitcher", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM(StripeHeight, 0.000001, 1.0, 0.1)
    PARAM(Advance, 0.0, 1.0, 0.1)
    PARAM(NotchHeight,  0.0, 1.0, 0.2)
    PARAM(SlideStripes,  0.0, 1.0, 0.0, FF_TYPE_BOOLEAN)
    PARAM(RepeatY, 0.0, 1.0, 0.0)
    PARAM(RepeatX, 0.0, 1.0, 0.0)
    PARAM(StretchX, 0.0, 1.0, 0.0)
    PARAM(OffsetY, 0.0, 1.0, 0.0)
    PARAM(OffsetX, 0.0, 1.0, 0.0)
    PARAM(Mix, 0.0, 1.0, 0.0)
END_SHADER_PARAMETERS()


