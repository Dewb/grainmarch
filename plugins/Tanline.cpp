//
//  Tanline.cpp
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include "Tanline.glsl"

DECLARE_PLUGIN(EffectPlugin, "DBTN", "Tanline", "Platonic scaline glitcher", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

void ShaderPlugin::InitParameters()
{
    m_parameters.push_back(Parameter("StripeHeight", 0.000001, 1.0, 0.1));
    m_parameters.push_back(Parameter("Advance", 0.0, 1.0, 0.1));
    m_parameters.push_back(Parameter("NotchHeight",  0.0, 1.0, 0.2));
    m_parameters.push_back(Parameter("SlideStripes",  0.0, 1.0, 0.0, FF_TYPE_BOOLEAN));
    m_parameters.push_back(Parameter("RepeatY", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("RepeatX", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("StretchX", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("OffsetY", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("OffsetX", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("Mix", 0.0, 1.0, 0.0));
}


