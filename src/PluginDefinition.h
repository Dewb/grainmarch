#pragma once

#ifdef WIN32
#include <windows.h>
#include <wingdi.h>
#undef max
#undef min
#endif

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

typedef std::vector<std::tuple<GLint, float, float*>> ManualUniformList;

class Parameter {
public:
    Parameter(string name, float min, float max, float value, int type = FF_TYPE_STANDARD, bool isShader = true, bool shouldRandomize = true, ParamAction action = nullptr, char* textValue = nullptr);
    float GetScaledValue() const;
    const char* GetTextValue() const;
    void SetTextValue(const char* value);

    string Name;
    int Type;
    float Value;
    string TextValue;

    float RangeMin;
    float RangeMax;
    
    ParamAction Action;
    float ActionData[4];
    
    bool IsShaderUniform;
    bool ShouldRandomize;
    float LastNonRandomValue;
    
    GLint UniformLocation;
};

class ShaderPlugin :
public CFreeFrameGLPlugin
{
public:
    ShaderPlugin(int nInputs);
    
    virtual ~ShaderPlugin() {}
    virtual void Initialize();
    virtual void UpdateUniforms(); 
    virtual void EmitGeometry();
    
    void InitParameters();

	virtual FFResult SetFloatParameter(unsigned int index, float value);
	virtual float GetFloatParameter(unsigned int index);
	virtual FFResult SetTextParameter(unsigned int index, const char *value);
	virtual char* GetTextParameter(unsigned int index);

	virtual FFResult ProcessOpenGL(ProcessOpenGLStruct* pGL);
    virtual FFResult InitGL(const FFGLViewportStruct *vp);
    virtual FFResult DeInitGL();
    virtual FFResult SetTime(double time);
    
    void ManuallyBindUniformFloat(string Name, float* pValue);
    void ManuallyBindUniformFloatArray(string Name, float count, float* pValue);
    
protected:	
	int m_initResources;
    FFGLShader m_shader;
    
    ParamList m_parameters;
    ManualUniformList m_floatUniforms;
    ManualUniformList m_floatArrayUniforms;

    float m_resolutionX;
    float m_resolutionY;
    GLint m_resolutionXLocation;
    GLint m_resolutionYLocation;
    
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
FFResult __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
{
    *ppOutInstance = new PluginType();
    if (*ppOutInstance != NULL)
        return FF_SUCCESS;
    return FF_FAIL;
}

#define DECLARE_PLUGIN(class, id, name, description, about) \
static CFFGLPluginInfo PluginInfo ( \
CreateInstance<class>, id, name, 1, 6, 1, 100, class::Type, description, about);

class ParameterListAdder {
public:
    ParameterListAdder(ParamList& list, Parameter param) {
        list.push_back(param);
    }
};

#define BEGIN_SHADER_PARAMETERS() namespace Param { \
ParamList macroParameters;
#define PARAM(paramID, ...) const unsigned int paramID = macroParameters.size(); \
ParameterListAdder adder##paramID(macroParameters, Parameter(#paramID, __VA_ARGS__));
#define END_SHADER_PARAMETERS() }

#define GetScaled(paramID) m_parameters[paramID].GetScaledValue()

namespace Param {
    extern ParamList macroParameters;
};

extern char vertexShaderCode[];
extern char fragmentShaderCode[];

extern float dice();

extern ParamAction AccumulateAction;


