#pragma once

#include <FFGLPluginSDK.h>
#include <FFGLLib.h>
#include <FFGLShader.h>

#include <string>
#include <vector>
#include <functional>


using namespace std;

class Parameter;
typedef std::vector<Parameter> ParamList;
typedef std::function<void(Parameter&, float, ParamList&)> ParamAction;

class Parameter {
public:
    Parameter(string name, float min, float max, float value, int type = FF_TYPE_STANDARD, bool isShader = true, ParamAction action = nullptr);
    float GetScaledValue() const;
    
    string Name;
    float Value;
    int Type;

    bool IsShaderUniform;
    GLint UniformLocation;
    ParamAction Action;
    
    float RangeMin;
    float RangeMax;
};

class ShaderPlugin :
public CFreeFrameGLPlugin
{
public:
    ShaderPlugin(int nInputs);
    
    virtual ~ShaderPlugin() {}
    virtual void InitParameters();
    virtual void EmitGeometry();
    
	DWORD SetParameter(const SetParameterStruct* pParam);
	DWORD GetParameter(DWORD dwIndex);
	DWORD ProcessOpenGL(ProcessOpenGLStruct* pGL);
    DWORD InitGL(const FFGLViewportStruct *vp);
    DWORD DeInitGL();
    DWORD SetTime(double time);
	

protected:	
	int m_initResources;
	FFGLExtensions m_extensions;
    FFGLShader m_shader;
    
    ParamList m_parameters;

    double m_startTime;
    double m_time;
    
    GLint m_timeLocation;
    float m_resolutionX;
    float m_resolutionY;
    GLint m_resolutionXLocation;
    GLint m_resolutionYLocation;
    
    bool m_HostSupportsSetTime;
    
    int m_nInputs;
    GLint* m_inputTextureLocationArray;
    FFGLTexCoords m_texDimensions;
    float m_aspectRatio;
};

class SourcePlugin : public ShaderPlugin
{
public:
    SourcePlugin();
    static int Type;
};

class EffectPlugin : public ShaderPlugin
{
public:
    EffectPlugin();
    static int Type;
};

template <class PluginType>
DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
{
    *ppOutInstance = new PluginType();
    if (*ppOutInstance != NULL)
        return FF_SUCCESS;
    return FF_FAIL;
}

#define DECLARE_PLUGIN(class, id, name, description, about) \
static CFFGLPluginInfo PluginInfo ( \
CreateInstance<class>, id, name, 1, 500, 1, 100, class::Type, description, about);

#define BEGIN_SHADER_PARAMETERS() ParamList shaderParameters = {
#define PARAM(...) Parameter(__VA_ARGS__),
#define END_SHADER_PARAMETERS() };

extern char vertexShaderCode[];
extern char fragmentShaderCode[];
extern ParamList shaderParameters;

extern float dice();

void update_time(double *t, const double t0);


