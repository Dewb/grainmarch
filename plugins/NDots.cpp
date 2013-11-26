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

    void drawLine(float x1, float y1, float x2, float y2) {
        float stretch = m_parameters[4].GetScaledValue();
        float length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        float dx = (x2 - x1) / length;
        float dy = (y2 - y1) / length;
        float sx = dx * length * (1 - stretch) * 0.5;
        float sy = dy * length * (1 - stretch) * 0.5;
        x1 += sx;
        y1 += sy;
        x2 -= sx;
        y2 -= sy;
        length *= stretch;
        
        float jitterAmount = m_parameters[5].GetScaledValue();
        float jitterSegments = m_parameters[6].GetScaledValue();
        if (jitterAmount > 0) {
            for (int ii = 0; ii < jitterSegments - 1; ii++) {
                float a = (dice() - 0.5) * jitterAmount;
                float xn = x1 + dx * length / jitterSegments - dy * a;
                float yn = y1 + dy * length / jitterSegments + dx * a;
                drawSingleLineSegment(x1, y1, xn, yn);
                x1 = xn;
                y1 = yn;
            }
        }
        
        drawSingleLineSegment(x1, y1, x2, y2);
    }
    
    void drawSingleLineSegment(float x1, float y1, float x2, float y2) {
        
        float lineWidth = m_parameters[3].GetScaledValue();
        bool solidLines = (bool)m_parameters[8].GetScaledValue();
        
        float length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        float dx = (x2 - x1) / length;
        float dy = (y2 - y1) / length;
        
        if (solidLines) {
            float wx = dy * lineWidth * -0.5;
            float wy = dx * lineWidth * 0.5;
            glBegin(GL_QUADS);
            glVertex2f(x1 + wx, y1 + wy);
            glVertex2f(x1 - wx, y1 - wy);
            glVertex2f(x2 - wx, y2 - wy);
            glVertex2f(x2 + wx, y2 + wy);
            glEnd();
        } else {
            glBegin(GL_LINES);
            for (float kk = -1.0; kk < 1.0; kk = kk + 0.05) {
                float wx = dy * kk * lineWidth * -0.5;
                float wy = dx * kk * lineWidth * 0.5;
                glVertex2f(x1 + wx, y1 + wy);
                glVertex2f(x2 + wx, y2 + wy);
            }
            glEnd();
        }

    }
    
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
        bool solidDots = (bool)m_parameters[7].GetScaledValue();
        float facets = m_parameters[9].GetScaledValue();
        float rotation = m_parameters[10].GetScaledValue();
        
        glRotatef(rotation, 0, 0, 1);
        
        for (int ii=0; ii < numDots; ii++) {
            float x1 = ngonRadius * cos(2 * PI * ii / numDots);
            float y1 = ngonRadius * sin(2 * PI * ii / numDots);
            
            for (int jj=0; jj < ii; jj++) {
                float x2 = ngonRadius * cos(2 * PI * jj / numDots);
                float y2 = ngonRadius * sin(2 * PI * jj / numDots);
                drawLine(x1, y1, x2, y2);
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

DECLARE_PLUGIN(NDotsPlugin, "DBND", "DotLineCrimes", "Dots and lines", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM("NumDots", 0.0, 12.0, 2.0)
    PARAM("DotRadius", 0.0, 0.5, 0.1)
    PARAM("NgonRadius", 0.0, 2.0, 0.5)
    PARAM("LineWidth", 0.0, 0.4, 0.05)
    PARAM("LineStretch", 0.0, 1.0, 0.7)
    PARAM("JitterAmount", 0.0, 1.0, 0.0)
    PARAM("JitterSegments", 2.0, 10.0, 4.0)
    PARAM("SolidDots", 0.0, 1.0, 1.0, FF_TYPE_BOOLEAN)
    PARAM("SolidLines", 0.0, 1.0, 1.0, FF_TYPE_BOOLEAN)
    PARAM("DotSmoothness", 0.0, 36.0, 36.0)
    PARAM("Rotation", -180.0, 180.0, 0.0)
END_SHADER_PARAMETERS()


