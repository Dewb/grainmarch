//
//  Paik-Abe.cpp
//  GrainmarchFFGL
//
//  Created by Michael Dewberry on 3/7/14.
//
//

#include "PluginDefinition.h"

#include <GLUT/GLUT.h>
#include <cmath>

#include "Paik-Abe.glsl"

#define PI 3.14159262

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

BEGIN_SHADER_PARAMETERS()
PARAM(FrameRate, 0.0, 60.0, 30.0, FF_TYPE_STANDARD, false)
PARAM(Scanlines, 0.0, 1000.0, 525.0, FF_TYPE_STANDARD, false)
PARAM(SamplesPerFrame, 0.0, 100000.0, 20000.0, FF_TYPE_STANDARD, false)
PARAM(StandardYokeX, -2.0, 2.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(StandardYokeY, -2.0, 2.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(SineFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(CosFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(TriFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(SineAmpX, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(SineAmpY, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(SineFreqX, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(SineFreqY, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(CosAmpX, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(CosAmpY, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(CosFreqX, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(CosFreqY, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(TriAmpX, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(TriAmpY, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(TriFreqX, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(TriFreqY, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(DampingX, 0.0, 20.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(DampingY, 0.0, 20.0, 0.0, FF_TYPE_STANDARD, false)

END_SHADER_PARAMETERS()

class PaikAbePlugin : public EffectPlugin
{
public:
    PaikAbePlugin() {}
    class Point {
    public:
        Point() : x(0), y(0) {};
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
    
    void drawLine(Point a, Point b, Point ta, Point tb, float lineWidth) {
        glMultiTexCoord2f(0, (ta.x + 1) / 2, (ta.y + 1) / 2);
        glVertex2f(a.x, a.y);
        glMultiTexCoord2f(0, (tb.x + 1) / 2, (tb.y + 1) / 2);
        glVertex2f(b.x, b.y);
    }
    
    double horizontalBeamFunction(double t, double linePeriod) {
        return 2.0 * fmod(t, linePeriod) / linePeriod - 1.0;
    }
    
    double verticalBeamFunction(double t, double linePeriod, double lines) {
        return 2.0 * floor(t / linePeriod) / lines - 1.0;
    }
    
    double tri(double t) {
        return 2.0 * fabs(2.0 * (t - floor(t + 0.5))) - 1.0;
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
        
        double linesPerSecond = GetScaled(Param::FrameRate) * GetScaled(Param::Scanlines);
        double linePeriod = 1.0 / linesPerSecond;
        double framePeriod = 1.0 / GetScaled(Param::FrameRate);
        double advance = framePeriod / GetScaled(Param::SamplesPerFrame);
        
        double t = 0;
        glBegin(GL_LINES);
        glLineWidth(3.0);
        Point lastPoint, currentPoint, lastSourcePoint, currentSourcePoint;
        
        while (t < framePeriod)
        {
            double h = horizontalBeamFunction(t, linePeriod);
            double v = verticalBeamFunction(t, linePeriod, GetScaled(Param::Scanlines));
            
            double x = (GetScaled(Param::StandardYokeX) * h +
                        GetScaled(Param::SineAmpX) * sin(t * GetScaled(Param::SineFreq) * pow(2.0, GetScaled(Param::SineFreqX))) +
                        GetScaled(Param::CosAmpX) * cos(t * GetScaled(Param::CosFreq) * pow(2.0, GetScaled(Param::CosFreqX))) +
                        GetScaled(Param::TriAmpX) * tri(t * GetScaled(Param::TriFreq) * pow(2.0, GetScaled(Param::TriFreqX)))) *
                        exp(-1.0 * GetScaled(Param::DampingX) * t);
            double y = (GetScaled(Param::StandardYokeY) * v +
                        GetScaled(Param::SineAmpY) * sin(t * GetScaled(Param::SineFreq) * pow(2.0, GetScaled(Param::SineFreqY))) +
                        GetScaled(Param::CosAmpY) * cos(t * GetScaled(Param::CosFreq) * pow(2.0, GetScaled(Param::CosFreqY))) +
                        GetScaled(Param::TriAmpY) * sin(t * GetScaled(Param::TriFreq) * pow(2.0, GetScaled(Param::TriFreqY)))) *
                        exp(-1.0 * GetScaled(Param::DampingY) * t);

            currentPoint = Point(x, y);
            currentSourcePoint = Point(h, v);
            
            if (t != 0.0)
                drawLine(lastPoint, currentPoint, lastSourcePoint, currentSourcePoint, 2.0);

            lastPoint = currentPoint;
            lastSourcePoint = currentSourcePoint;

            t += advance;
        }
        glEnd();

        glPopMatrix();
        glPopAttrib();
    }
};

DECLARE_PLUGIN(PaikAbePlugin, "DBPA", "Paik-Abe", "remix/riff/recreation of the classic Paik-Abe video synthesizer", "by Michael Dewberry - dewb.org")




