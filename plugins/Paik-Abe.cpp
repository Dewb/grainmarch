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
PARAM(Quality, 0.0, 100000.0, 20000.0, FF_TYPE_STANDARD, false, false)
PARAM(Scanlines, 0.0, 1000.0, 525.0, FF_TYPE_STANDARD, false)
PARAM(S_Angle, 0.0, 2 * PI, 0.0, FF_TYPE_STANDARD, false)
PARAM(SineFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(CosFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(TriFreq, 0.0, 1000.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(Mix, 0.0, 1.0, 1.0, FF_TYPE_STANDARD, false, false)
PARAM(H, -2.0, 2.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(V, -2.0, 2.0, 1.0, FF_TYPE_STANDARD, false)
PARAM(H_SineAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_SineFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_CosAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_CosFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_TriAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_TriFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H_Damping, 0.0, 100.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_SineAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_SineFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_CosAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_CosFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_TriAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_TriFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(V_Damping, 0.0, 100.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_SineAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_SineFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_CosAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_CosFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_TriAmp, -2.0, 2.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_TriFreq, -3.0, 3.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_Damping, 0.0, 100.0, 0.0, FF_TYPE_STANDARD, false)

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
    
    double lerp(double lo, double hi, double mix) {
        return lo + mix * (hi - lo);
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
        
        double frameRate = 1.0;
        double linesPerSecond = frameRate * GetScaled(Param::Scanlines);
        double linePeriod = 1.0 / linesPerSecond;
        double framePeriod = 1.0 / frameRate;
        double advance = framePeriod / GetScaled(Param::Quality); // e.g. samples per frame
        
        double t = 0;
        glBegin(GL_LINES);
        glLineWidth(3.0);
        Point lastPoint, currentPoint, lastSourcePoint, currentSourcePoint;
        
        double mix = GetScaled(Param::Mix);
        double hFactor = lerp(1.0, GetScaled(Param::H), mix);
        double hSine = GetScaled(Param::H_SineAmp) * mix;
        double hCos = GetScaled(Param::H_CosAmp) * mix;
        double hTri = GetScaled(Param::H_TriAmp) * mix;
        double hSineFreq = GetScaled(Param::SineFreq) * pow(2.0, GetScaled(Param::H_SineFreq));
        double hCosFreq = GetScaled(Param::CosFreq) * pow(2.0, GetScaled(Param::H_CosFreq));
        double hTriFreq = GetScaled(Param::TriFreq) * pow(2.0, GetScaled(Param::H_TriFreq));
        double hDamping = GetScaled(Param::H_Damping);
        
        double vFactor = lerp(1.0, GetScaled(Param::V), mix);
        double vSine = GetScaled(Param::V_SineAmp) * mix;
        double vCos = GetScaled(Param::V_CosAmp) * mix;
        double vTri = GetScaled(Param::V_TriAmp) * mix;
        double vSineFreq = GetScaled(Param::SineFreq) * pow(2.0, GetScaled(Param::V_SineFreq));
        double vCosFreq = GetScaled(Param::CosFreq) * pow(2.0, GetScaled(Param::V_CosFreq));
        double vTriFreq = GetScaled(Param::TriFreq) * pow(2.0, GetScaled(Param::V_TriFreq));
        double vDamping = GetScaled(Param::V_Damping);
        
        double sAngle = PI/8.0 + GetScaled(Param::S_Angle);
        double sSine = GetScaled(Param::S_SineAmp) * mix;
        double sCos = GetScaled(Param::S_CosAmp) * mix;
        double sTri = GetScaled(Param::S_TriAmp) * mix;
        double sSineFreq = GetScaled(Param::SineFreq) * pow(2.0, GetScaled(Param::S_SineFreq));
        double sCosFreq = GetScaled(Param::CosFreq) * pow(2.0, GetScaled(Param::S_CosFreq));
        double sTriFreq = GetScaled(Param::TriFreq) * pow(2.0, GetScaled(Param::S_TriFreq));
        double sDamping = GetScaled(Param::S_Damping);
        
        while (t < framePeriod)
        {
            double h = horizontalBeamFunction(t, linePeriod);
            double v = verticalBeamFunction(t, linePeriod, GetScaled(Param::Scanlines));
            
            double x = (hFactor * h +
                        hSine * sin(t * hSineFreq) +
                        hCos * cos(t * hCosFreq) +
                        hTri * tri(t * hTriFreq)) *
                        lerp(1.0, exp(-1.0 * hDamping * t), mix);
            
            double y = (vFactor * v +
                        vSine * sin(t * vSineFreq) +
                        vCos * cos(t * vCosFreq) +
                        vTri * sin(t * vTriFreq)) *
                        lerp(1.0, exp(-1.0 * vDamping * t), mix);

            double s = (sSine * sin(t * sSineFreq) +
                        sCos * cos(t * sCosFreq) +
                        sTri * sin(t * sTriFreq)) *
                        lerp(1.0, exp(-1.0 * sDamping * t), mix);
            
            currentPoint = Point(x + s * cos(sAngle), y + s * sin(sAngle));
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




