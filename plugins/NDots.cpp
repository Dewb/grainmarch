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


class NDotsPlugin : public SourcePlugin
{
public:
    NDotsPlugin() {}
    
    virtual void EmitGeometry() {
        
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        glPushMatrix();
        
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
        bool solidDots = (bool)m_parameters[6].GetScaledValue();
        bool solidLines = (bool)m_parameters[7].GetScaledValue();
        float facets = m_parameters[8].GetScaledValue();
        float rotation = m_parameters[9].GetScaledValue();
        
        glRotatef(rotation, 0, 0, 1);
        
        for (int ii=0; ii < numDots; ii++) {
            float x1 = ngonRadius * cos(2 * PI * ii / numDots);
            float y1 = ngonRadius * sin(2 * PI * ii / numDots);
            
            for (int jj=0; jj < ii; jj++) {
                float x2 = ngonRadius * cos(2 * PI * jj / numDots);
                float y2 = ngonRadius * sin(2 * PI * jj / numDots);
                float length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
                float dx = (x2 - x1) / length;
                float dy = (y2 - y1) / length;
                float sx = dx * length * (1 - stretch);
                float sy = dy * length * (1 - stretch);

                if (solidLines) {
                    float wx = dy * lineWidth * -0.5;
                    float wy = dx * lineWidth * 0.5;
                    glBegin(GL_QUADS);
                    glVertex2f(x1 + wx + sx, y1 + wy + sy);
                    glVertex2f(x1 - wx + sx, y1 - wy + sy);
                    glVertex2f(x2 - wx - sx, y2 - wy - sy);
                    glVertex2f(x2 + wx - sx, y2 + wy - sy);
                    glEnd();
                } else {
                    glBegin(GL_LINES);
                    for (float kk = -1.0; kk < 1.0; kk = kk + 0.05) {
                        float wx = dy * kk * lineWidth * -0.5;
                        float wy = dx * kk * lineWidth * 0.5;
                        glVertex2f(x1 + wx + sx, y1 + wy + sy);
                        glVertex2f(x2 + wx - sx, y2 + wy - sy);
                    }
                    glEnd();
                }
            }
        }
        
        for (int ii=0; ii < numDots; ii++) {
            float x = ngonRadius * cos(2 * PI * ii / numDots);
            float y = ngonRadius * sin(2 * PI * ii / numDots);
            glPushMatrix();
            glTranslatef(x, y, 0);
            if (solidDots) {
                glutSolidSphere(dotRadius, facets, facets);
            } else {
                glutWireSphere(dotRadius, facets, facets);
            }
            glPopMatrix();
        }

        glPopMatrix();
        glPopAttrib();
    }
};

DECLARE_PLUGIN(NDotsPlugin, "DBND", "NDots", "Dots and lines", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM("NumDots", 0.0, 12.0, 2.0)
    PARAM("DotRadius", 0.0, 0.5, 0.1)
    PARAM("NgonRadius", 0.0, 2.0, 0.5)
    PARAM("LineWidth", 0.0, 0.4, 0.05)
    PARAM("LineStretch", 0.5, 1.0, 0.7)
    PARAM("LineNoise", 0.0, 1.0, 0.0)
    PARAM("SolidDots", 0.0, 1.0, 1.0, FF_TYPE_BOOLEAN)
    PARAM("SolidLines", 0.0, 1.0, 1.0, FF_TYPE_BOOLEAN)
    PARAM("DotSmoothness", 0.0, 36.0, 36.0)
    PARAM("Rotation", -180.0, 180.0, 0.0)
END_SHADER_PARAMETERS()


