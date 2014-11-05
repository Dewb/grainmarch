//
//  Marblephase.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 10/27/14.
//
//

#include "PluginDefinition.h"
#include "Marblephase.glsl"
#include "default_vertex_shader.glsl"
#include <cmath>

#define PI 3.1415926535897932384626433832795

/*
const float coeffs_aold[32] = {
    1.0, -21.0, 35.0, -7.0, -6.0, 1.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    -7.0, 35.0, 4.0, -21.0, -4.0, 1.0, 2.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
 */

const float coeffs_a[32] = {
    -7.0, 0.0, 1.0, -21.0, 35.0, 0.0, 0.0, 0.0,
    -6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    -7.0, 1.0, 35.0, -21.0, 0.0, 0.0, 0.0, 0.0,
    4.0, -4.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0
};

const float coeffs_b[32] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 12.0, 0.0, 12.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 12.0, -12.0, 0.0, 0.0
};

const float coeffs_c[32] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    12.0, 12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    12.0, 12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

const float coeffs_d[32] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    8.0, 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    8.0, 8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};


// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
PARAM(Zoom, 0, 1, 0.5)
PARAM(Direction, 0, 2 * PI, 0)
PARAM(Imprecision, 0, 1.0, 0)
PARAM(Degree, 2.0, 16.0, 16.0, FF_TYPE_STANDARD, false);
PARAM(Morph1, 0, 1, 0, FF_TYPE_STANDARD, false);
PARAM(Morph2, 0, 1, 0, FF_TYPE_STANDARD, false);
PARAM(Rotation, 0, 2 * PI, 0);
PARAM(Stripes, 0, 0.5, 0);
PARAM(StripePeriod, -1.5, 1.5, 0, FF_TYPE_STANDARD, false);
PARAM(ColorMode, 0, 1, 0);
PARAM(HueLimit, 0, 1, 1);
PARAM(HueShift, 0, 1, 0);
PARAM(Saturation, 0, 1, 1);
PARAM(Overexpose, 0.0, 5.0, 0.0);
END_SHADER_PARAMETERS()

float mix(float a, float b, float s) {
    return a + s * (b - a);
}

class MarblephasePlugin : public SourcePlugin
{
public:
    float K[32];
    float LogStripePeriod;
    float LogOverexpose;
    
    MarblephasePlugin() {}
    virtual void Initialize() {
        ManuallyBindUniformFloatArray("K", 32, K);
        ManuallyBindUniformFloat("LogStripePeriod", &LogStripePeriod);
        ManuallyBindUniformFloat("LogOverexpose", &LogOverexpose);
    };
    virtual void UpdateUniforms() {
        int degree = ceil(GetScaled(Param::Degree));
        for(int i = 0; i < 32; i++) {
            if (16 -  (i % 16) > degree) {
                K[i] = 0;
            } else {
                float m1 = GetScaled(Param::Morph1);
                float x = mix(coeffs_a[i], coeffs_b[i], m1);
                float y = mix(coeffs_c[i], coeffs_d[i], m1);
                K[i] = mix(x, y, GetScaled(Param::Morph2));
            }
        }

        LogStripePeriod = pow(10, GetScaled(Param::StripePeriod));
        LogOverexpose = pow(2, GetScaled(Param::Overexpose));
    };
};

DECLARE_PLUGIN(MarblephasePlugin, "DBPH", "Marblephase", "Complex phase portrait experiment", "by Michael Dewberry - dewb.org")




