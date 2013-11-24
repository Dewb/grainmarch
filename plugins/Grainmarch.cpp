//
//  Grainmarch.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 11/21/13.
//
//

#include "PluginDefinition.h"
#include "Grainmarch.glsl"

DECLARE_PLUGIN(SourcePlugin, "GMAR", "Grainmarch", "Grainy fractal raymarching", "by Michael Dewberry - dewb.org\nBased on Shadertoy shaders by Syntopia and Kali")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

void ShaderPlugin::InitParameters()
{
    m_parameters.push_back(Parameter("FieldOfView", 0.1, 10.0, 1.0));
    m_parameters.push_back(Parameter("Iterations", 1, 30.0, 7));
    m_parameters.push_back(Parameter("Scale", 1.0, 10.0, 3.0));
    m_parameters.push_back(Parameter("ZoomSpeed", -2.0, 2.0, 0.5));
    m_parameters.push_back(Parameter("NonLinearPerspective", -10.0, 10.0, 2.0));
    m_parameters.push_back(Parameter("NLPOnly", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("OffsetX", 0.0, 1.0, 0.5));
    m_parameters.push_back(Parameter("OffsetY", 0.0, 1.0, 0.5));
    m_parameters.push_back(Parameter("OffsetZ", 0.0, 1.0, 0.5));
    m_parameters.push_back(Parameter("Color1R", 0.0, 1.0, 1.0));
    m_parameters.push_back(Parameter("Color1G", 0.0, 1.0, 1.0));
    m_parameters.push_back(Parameter("Color1B", 0.0, 1.0, 0.858824));
    m_parameters.push_back(Parameter("Color2R", 0.0, 1.0, 0.0));
    m_parameters.push_back(Parameter("Color2G", 0.0, 1.0, 0.333333));
    m_parameters.push_back(Parameter("Color2B", 0.0, 1.0, 1.0));
    m_parameters.push_back(Parameter("Ambient", 0.0, 1.0, 0.32184));
    m_parameters.push_back(Parameter("Diffuse", 0.0, 1.0, 0.7));
    m_parameters.push_back(Parameter("Jitter", 0.0, 0.2, 0.05));
    m_parameters.push_back(Parameter("IterationRotation", -10.0, 10.0, 4.0));
    m_parameters.push_back(Parameter("IterationRotationLFO", -1.0, 1.0, 0.125));
    m_parameters.push_back(Parameter("IterationRotationLFOIntensity", 0.0, 10.0, 2.0));
    m_parameters.push_back(Parameter("NLPRotationLFO", -1.0, 1.0, 0.25));
}


