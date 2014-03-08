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

#include "Paik-Abe.glsl"

#define PI 3.14159262

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
PARAM(Time, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(FrameRate, 0.0, 60.0, 30.0, FF_TYPE_STANDARD, false)
PARAM(Scanlines, 0.0, 1000.0, 525.0, FF_TYPE_STANDARD, false)
PARAM(SamplesPerFrame, 0.0, 100000.0, 20000.0, FF_TYPE_STANDARD, false)
END_SHADER_PARAMETERS()

class PaikAbePlugin : public SourcePlugin
{
public:
    PaikAbePlugin() {}
    class Point {
    public:
        Point(float xx, float yy) : x(xx), y(yy) {};
        float x;
        float y;
    };
    
    vector<Point> beamPath;
    
    /*
     void drawLine(float x1, float y1, float x2, float y2, float lineWidth) {
        
        float length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        float dx = (x2 - x1) / length;
        float dy = (y2 - y1) / length;
        
            float wx = dy * lineWidth * -0.5;
            float wy = dx * lineWidth * 0.5;
        
            glVertex2f(x1 + wx, y1 + wy);
            glVertex2f(x1 - wx, y1 - wy);
            glVertex2f(x2 - wx, y2 - wy);
            glVertex2f(x2 + wx, y2 + wy);
        

    }
     */
    
    void drawLine(float x1, float y1, float x2, float y2, float lineWidth) {
        glVertex2f((2.0 * x1) - 1.0, (2.0 * y1) - 1.0);
        glVertex2f((2.0 * x2) - 1.0, (2.0 * y2) - 1.0);
    }
    
    double horizontalBeamFunction(double t, double linePeriod) {
        return fmod(t, linePeriod) / linePeriod;
    }
    
    double verticalBeamFunction(double t, double linePeriod) {
        return floor(t / linePeriod) * linePeriod; // this is wrong, but it looks cool
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
        
        beamPath.clear();
        
        double linesPerSecond = GetScaled(Param::FrameRate) * GetScaled(Param::Scanlines);
        double linePeriod = 1.0 / linesPerSecond;
        double framePeriod = 1.0 / GetScaled(Param::FrameRate);
        double advance = framePeriod / GetScaled(Param::SamplesPerFrame);
        
        double t = 0;
        glBegin(GL_LINES);
        Point lastPoint(0, 0);
        Point currentPoint = lastPoint;
        while (t < framePeriod)
        {
            lastPoint = currentPoint;
            currentPoint = Point(horizontalBeamFunction(t, linePeriod), verticalBeamFunction(t, linePeriod));
            drawLine(lastPoint.x, lastPoint.y, currentPoint.x, currentPoint.y, 2.0);
            t += advance;
        }
        glEnd();

        glPopMatrix();
        glPopAttrib();
    }
};

DECLARE_PLUGIN(PaikAbePlugin, "DBPA", "Paik-Abe", "remix/riff/recreation of the classic Paik-Abe video synthesizer", "by Michael Dewberry - dewb.org")




