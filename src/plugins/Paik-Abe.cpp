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

#include "exprtk/exprtk.hpp"
#include <iostream>


#define PI 3.14159262

// Parameter(uniform name, minimum value, maximum value, default value, optional type (default standard/float.))

char DefaultHFunction[] = "sin(t)";
char DefaultVFunction[] = "cos(t)";
char DefaultSFunction[] = "0";

#define MAX_FUNCTION_LENGTH 255

BEGIN_SHADER_PARAMETERS()
PARAM(Mix, 0.0, 1.0, 1.0, FF_TYPE_STANDARD, false, false)
PARAM(Zoom, -1.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(Rotate, 180, -180, 0.0, FF_TYPE_STANDARD, false)
PARAM(Scanlines, 0.0, 1000.0, 525.0, FF_TYPE_STANDARD, false)
PARAM(BeamWidth, 0.0002, 0.1, 0.002, FF_TYPE_STANDARD, false)
PARAM(Intensity, 0.0, 1.0, 0.3, FF_TYPE_STANDARD, true)
PARAM(Quality, 0.0, 3000.0, 1000.0, FF_TYPE_STANDARD, false, false)
PARAM(Noise, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(H, 0.0, 0.0, 0.0, FF_TYPE_TEXT, false, false, nullptr, DefaultHFunction)
PARAM(V, 0.0, 0.0, 0.0, FF_TYPE_TEXT, false, false, nullptr, DefaultVFunction)
PARAM(S, 0.0, 0.0, 0.0, FF_TYPE_TEXT, false, false, nullptr, DefaultSFunction)
PARAM(A, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(B, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(C, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(D, 0.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(S_Angle, 0.0, 2 * PI, 0.0, FF_TYPE_STANDARD, false)
PARAM(Damping, 0.0, 100.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(BlankingFreq, -1.0, 1.0, 0.0, FF_TYPE_STANDARD, false)
PARAM(BlankingDuty, 0.0, 1.0, 0.02, FF_TYPE_STANDARD, false)

END_SHADER_PARAMETERS()

template <typename T>
class ExpressionWrapper {
public:

    string expressionString;

    typedef exprtk::symbol_table<T>     symbol_table_t;
    typedef exprtk::expression<T>       expression_t;
    typedef exprtk::parser<T>           parser_t;
    typedef typename parser_t::dependent_entity_collector::symbol_t symbol_t;

    parser_t parser;
    symbol_table_t symbol_table;
    expression_t expression;
    std::deque<symbol_t> symbol_list;

    bool isParsed;
    bool isBound;

    ExpressionWrapper() {
        expression.register_symbol_table(symbol_table);
        parser.dec().collect_variables() = true;
        parser.enable_unknown_symbol_resolver();
        isParsed = false;
        isBound = false;
    }

    void parse(string expr) {
        expressionString = expr;

        if (!parser.compile(expressionString, expression)) {
            cout << "Failed to compile expression: " << expressionString << ", error: " << parser.error() << "\n";
            return;
        }

        isParsed = true;
        parser.dec().symbols(symbol_list);
    }

    bool bind(const vector<string> formalParams, const vector<T> values) {

        for (std::size_t i = 0; i < symbol_list.size(); ++i) {
            symbol_t& symbol = symbol_list[i];

            switch (symbol.second) {
                case parser_t::e_st_variable:
                {
                    bool found = false;
                    for (int i = 0; i < formalParams.size(); i++) {
                        if (formalParams[i] == symbol.first) {
                            symbol_table.variable_ref(symbol.first) = values[i];
                            found = true;
                        }
                    }
                    if (!found) {
                        cout << "ERROR: Unknown symbol " << symbol.first << " in expression " << expressionString << "\n";
                        return false;
                    }
                }
                    break;
                default:
                    break;
            }
        }

        isBound = true;
        return true;
    }

    T getValue() {
        if (!isParsed) {
            cout << "ERROR: Asking for value of unparsed expression " << expressionString << "\n";
        } else if (!isBound) {
            cout << "ERROR: Asking for value of unbound expression " << expressionString << "\n";
        }
        return expression.value();
    }
};

typedef ExpressionWrapper<float> Expression;


class PaikAbePlugin : public EffectPlugin
{
public:

    char lastHFunction[255];
    char lastVFunction[255];
    char lastSFunction[255];

    Expression exprH;
    Expression exprV;
    Expression exprS;

    vector<string> expressionParams;
    vector<float> expressionParamValues;

    PaikAbePlugin() {
        expressionParams.push_back("t");
        expressionParams.push_back("A");
        expressionParams.push_back("B");
        expressionParams.push_back("C");
        expressionParams.push_back("D");
        expressionParams.push_back("a");
        expressionParams.push_back("b");
        expressionParams.push_back("c");
        expressionParams.push_back("d");
        for (float i = 0; i < expressionParams.size(); i++) {
            expressionParamValues.push_back(0);
        }
    }

    class Point {
    public:
        Point() : x(0), y(0) {};
        Point(float xx, float yy) : x(xx), y(yy) {};
        float x;
        float y;
    };
   
    void drawLineQuad(Point a, Point b, Point ta, Point tb, float lineWidth) {
        float length = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
        float dx = (b.x - a.x) / length;
        float dy = (b.y - a.y) / length;
        float wx = dy * lineWidth * -0.5;
        float wy = dx * lineWidth * 0.5;
        
        glTexCoord3f((ta.x + 1) / 2, (ta.y + 1) / 2, 0.0);
        glVertex2f(a.x + wx, a.y + wy);
        glTexCoord3f((ta.x + 1) / 2, (ta.y + 1) / 2, 1.0);
        glVertex2f(a.x - wx, a.y - wy);
        
        glTexCoord3f((tb.x + 1) / 2, (tb.y + 1) / 2, 1.0);
        glVertex2f(b.x - wx, b.y - wy);
        glTexCoord3f((tb.x + 1) / 2, (tb.y + 1) / 2, 0.0);
        glVertex2f(b.x + wx, b.y + wy);
    }
    
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
    
    bool beamBlankingFunction(double t, double hBlankPeriod, double hBlankDutyCycle) {
        return fmod(t, hBlankPeriod) / hBlankPeriod > (1.0 - hBlankDutyCycle);
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

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glPushMatrix();
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        float zoom = pow(16.0, -GetScaled(Param::Zoom));
        glRotatef(GetScaled(Param::Rotate), 0, 0, 1);
        glOrtho(-m_aspectRatio * zoom, m_aspectRatio * zoom, -1.0 * zoom, 1.0 * zoom, -20.0, 20.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        double frameRate = 1.0;
        double linesPerSecond = frameRate * GetScaled(Param::Scanlines);
        double linePeriod = 1.0 / linesPerSecond;
        double framePeriod = 1.0 / frameRate;
        double advance = framePeriod / GetScaled(Param::Quality); // e.g. samples per frame
        
        double hBlankingPeriod = linePeriod * pow(2.0, GetScaled(Param::BlankingFreq));
        double hBlankingDutyCycle = GetScaled(Param::BlankingDuty);
        
        double mix = GetScaled(Param::Mix);

        double hFactor = 1.0;
        //double hFactor = lerp(1.0, GetScaled(Param::H), mix);
        double damping = GetScaled(Param::Damping);

        expressionParamValues[1] = GetScaled(Param::A);
        expressionParamValues[2] = GetScaled(Param::B);
        expressionParamValues[3] = GetScaled(Param::C);
        expressionParamValues[4] = GetScaled(Param::D);
        expressionParamValues[5] = GetScaled(Param::A);
        expressionParamValues[6] = GetScaled(Param::B);
        expressionParamValues[7] = GetScaled(Param::C);
        expressionParamValues[8] = GetScaled(Param::D);

        if (strncmp(lastHFunction, GetParameterDisplay(Param::H), MAX_FUNCTION_LENGTH) != 0) {
            strncpy(lastHFunction, GetParameterDisplay(Param::H), MAX_FUNCTION_LENGTH);
            lastHFunction[MAX_FUNCTION_LENGTH-1] = 0;
            exprH.parse(lastHFunction);
        }

        if (strncmp(lastVFunction, GetParameterDisplay(Param::V), MAX_FUNCTION_LENGTH) != 0) {
            strncpy(lastVFunction, GetParameterDisplay(Param::V), MAX_FUNCTION_LENGTH);
            lastHFunction[MAX_FUNCTION_LENGTH-1] = 0;
            exprV.parse(lastVFunction);
        }

        double vFactor = 1.0;
        //double vFactor = lerp(1.0, GetScaled(Param::V), mix);

        double sAngle = PI/8.0 + GetScaled(Param::S_Angle);

        double lineWidth = GetScaled(Param::BeamWidth) * zoom;
        double noise = GetScaled(Param::Noise);
        
        double t = 0;
        Point lastPoint, currentPoint, lastSourcePoint, currentSourcePoint;

        glBegin(GL_QUADS);
        
        while (t < framePeriod)
        {
            double hSource = horizontalBeamFunction(t, linePeriod);
            double vSource = verticalBeamFunction(t, linePeriod, GetScaled(Param::Scanlines));

            expressionParamValues[0] = t / framePeriod;
            exprH.bind(expressionParams, expressionParamValues);
            exprV.bind(expressionParams, expressionParamValues);

            double h = exprH.getValue();
            double v = exprV.getValue();
            //cout << "(" << h << "," << v << ")\n";

            double x = (hFactor * h) *
                        lerp(1.0, exp(-1.0 * damping * t), mix);
            
            double y = (vFactor * v) *
                        lerp(1.0, exp(-1.0 * damping * t), mix);

            double s = 0.0;

            currentPoint = Point(x + s * cos(sAngle), y + s * sin(sAngle));
            currentSourcePoint = Point(hSource, vSource);
            
            if (t != 0.0 &&
                !beamBlankingFunction(t - advance, hBlankingPeriod, hBlankingDutyCycle) &&
                (noise == 0.0 || dice() > noise))
            {
                drawLineQuad(lastPoint, currentPoint, lastSourcePoint, currentSourcePoint, lineWidth);
            }
            
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




