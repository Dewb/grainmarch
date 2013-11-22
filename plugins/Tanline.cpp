//
//  Tanline.cpp
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"

static CFFGLPluginInfo PluginInfo (
                                   ShaderPlugin::CreateInstance,	// Create method
                                   "DBTN",								// Plugin unique ID
                                   "Tanline",					// Plugin name
                                   1,						   			// API major version number
                                   500,								  // API minor version number
                                   1,										// Plugin major version number
                                   000,									// Plugin minor version number
                                   FF_EFFECT,						// Plugin type
                                   "Platonic scanline glitcher",	// Plugin description
                                   "by Michael Dewberry - dewb.org" // About
                                   );


#include "Tanline.glsl"

void ShaderPlugin::CompileShader() {
    m_shader.Compile(vertexShaderCode, fragmentShaderCode);
}

ShaderType shaderType = Effect;

// To FFGLize an arbitrary shader, replace <shader>_frag.glsl and the parameters below.
// Parameter(uniform name, minimum value, maximum value, default value)
// Standard parameters/float uniforms only for now.

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


