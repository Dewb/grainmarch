//
//  Blockhead.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 11/24/13.
//
//

#include "PluginDefinition.h"

#include <GLUT/GLUT.h>
#include <cmath>

#include "NDots.glsl"

#define PI 3.14159262


class NDotsPlugin : public EffectPlugin
{
public:
    NDotsPlugin() {}
    
    virtual void EmitGeometry() {
        
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-m_aspectRatio/2, m_aspectRatio/2, -0.5, 0.5, -20.0, 20.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        int numDots = (int)floor(m_parameters[0].GetScaledValue());
        float dotRadius = m_parameters[1].GetScaledValue();
        float ngonRadius = m_parameters[2].GetScaledValue();
        float lineWidth = m_parameters[3].GetScaledValue();
        float stretch = m_parameters[4].GetScaledValue();
        
        for (int ii=0; ii < numDots; ii++) {
            float x = ngonRadius * cos(2 * PI * ii / numDots);
            float y = ngonRadius * sin(2 * PI * ii / numDots);
            glPushMatrix();
            glTranslatef(x, y, 0);
            glutSolidSphere(dotRadius, 36, 36);
            glPopMatrix();
            glBegin(GL_LINES);
            for (int jj=0; jj < ii; jj++) {
                float x2 = ngonRadius * cos(2 * PI * jj / numDots);
                float y2 = ngonRadius * sin(2 * PI * jj / numDots);
                float length = sqrt(pow(x2 - x, 2) + pow(y2 - y, 2));
                float dx = (x2 - x) / length;
                float dy = (y2 - y) / length;
                for (float kk = -1.0; kk < 1.0; kk = kk + 0.05) {
                    float wx = dy * kk * lineWidth * -1;
                    float wy = dx * kk * lineWidth;
                    float sx = dx * length * (1 - stretch);
                    float sy = dy * length * (1 - stretch);
                    glVertex2f(x + wx + sx, y + wy + sy);
                    glVertex2f(x2 + wx - sx, y2 + wy - sy);
                }
            }
            glEnd();
        }

        glPopAttrib();
    }
};

DECLARE_PLUGIN(NDotsPlugin, "DBND", "NDots", "Dots and lines", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM("NumDots", 0.0, 12.0, 2.0)
    PARAM("DotRadius", 0.0, 1.0, 0.15)
    PARAM("NgonRadius", 0.0, 2.0, 0.5)
    PARAM("LineWidth", 0.0, 0.5, 0.05)
    PARAM("LineStretch", 0.5, 1.0, 1.0)
END_SHADER_PARAMETERS()


