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

#include "Blockhead.glsl"

#define RAD_TO_DEG 57.29578


class BlockheadPlugin : public EffectPlugin
{
public:
    BlockheadPlugin() {}
    
    void drawFace(float fSize, int whichTex) {
        float width = m_texDimensions.s;
        float x = 0;
        float height = m_texDimensions.t;
        if (m_aspectRatio > 3) {
            width = m_texDimensions.s / 3;
            x = width * whichTex;
        }
        
        fSize /= 2.0;
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glMultiTexCoord2f(0, x, 0);
        glMultiTexCoord2f(1, 0, 0);
        glVertex3f(-fSize, -fSize, fSize);
        glMultiTexCoord2f(0, x + width, 0);
        glMultiTexCoord2f(1, 1, 0);
        glVertex3f(fSize, -fSize, fSize);
        glMultiTexCoord2f(0, x + width, height);
        glMultiTexCoord2f(1, 1, 1);
        glVertex3f(fSize, fSize, fSize);
        glMultiTexCoord2f(0, x, height);
        glMultiTexCoord2f(1, 0, 1);
        glVertex3f(-fSize, fSize, fSize);
        glEnd();
    }
    
    void drawCube(float fSize) {
        glPushMatrix();
        drawFace(fSize, 0);
        glRotatef (-90, 0, 1, 0);
        drawFace(fSize, 1);
        glRotatef (90, 1, 0, 0);
        drawFace(fSize, 2);
        glRotatef (90, 1, 0, 0);
        drawFace(fSize, 1);
        glRotatef (90, 1, 0, 0);
        drawFace(fSize, 2);
        glRotatef (-90, 0, 1, 0);
        drawFace(fSize, 0);
        glPopMatrix();
    }
    
    void drawColumn(int rows, float cubeSize, float spacing, float pitch, float pitchIncrement, float yaw) {
        float d = cubeSize + spacing;
        glTranslatef(0, (d * (rows - 1)) / 2.0, 0);
        for (int ii = 0; ii < rows; ii++) {
            glPushMatrix();
            glRotatef(pitch + pitchIncrement * ii, 1, 0, 0);
            glRotatef(yaw, 0, 1, 0);
            drawCube(cubeSize);
            glPopMatrix();
            glTranslatef(0, -d, 0);
        }
    }

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
                
        float blockSize = m_parameters[0].GetScaledValue();
        float spacing = m_parameters[1].GetScaledValue();
        int rows = (int)floor(m_parameters[2].GetScaledValue());
        int cols = (int)floor(m_parameters[3].GetScaledValue());
        float pitch = m_parameters[4].GetScaledValue();
        float yaw = m_parameters[5].GetScaledValue();
        float pitchIncrement = m_parameters[6].GetScaledValue();
        float yawIncrement = m_parameters[7].GetScaledValue();

        float d = blockSize + spacing;

        glPushMatrix();
        glTranslatef(-(d * (cols - 1)) / 2.0, 0, 0);
        
        for (int ii=0; ii < cols; ii++) {
            glPushMatrix();
            drawColumn(rows, blockSize, spacing, pitch, pitchIncrement, yaw + yawIncrement * ii);
            glPopMatrix();
            glTranslatef(d, 0, 0);
        }

        glPopMatrix();
        glPopAttrib();
    }
};

DECLARE_PLUGIN(BlockheadPlugin, "DBBH", "Blockhead", "Stack some cubes", "by Michael Dewberry - dewb.org")

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
    PARAM(BlockSize, 0.0, 1.5, 0.25, FF_TYPE_STANDARD, false)
    PARAM(Spacing, 0.0, 1.0, 0.02, FF_TYPE_STANDARD, false)
    PARAM(Rows, 0.0, 30.0, 5.0, FF_TYPE_STANDARD, false)
    PARAM(Columns, 0.0, 40.0, 11.0, FF_TYPE_STANDARD, false)
    PARAM(Pitch, -180.0, 180.0, 45.0, FF_TYPE_STANDARD, false)
    PARAM(Yaw, -180.0, 180.0, 45.0, FF_TYPE_STANDARD, false)
    PARAM(PitchIncrement, -90.0, 90.0, 0.0, FF_TYPE_STANDARD, false)
    PARAM(YawIncrement, -90.0, 90.0, 0.0, FF_TYPE_STANDARD, false)
    PARAM(Border, 0.0, 0.5, 0.05)
END_SHADER_PARAMETERS()


