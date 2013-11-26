#include <FFGL.h>
#include <FFGLLib.h>

#include "PluginDefinition.h"

#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

#include <sys/time.h>

float dice() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

Parameter::Parameter(string name, float min, float max, float value, int type, bool isShader, ParamAction action)
: Name(name)
, Type(type)
, RangeMin(min)
, RangeMax(max)
, IsShaderUniform(isShader)
, UniformLocation(-1)
, Action(action)
{
    Value = (value - min) / (max - min);
}

float Parameter::GetScaledValue() const {
    return RangeMin + Value * (RangeMax - RangeMin);
}

ShaderPlugin::ShaderPlugin(int nInputs)
: CFreeFrameGLPlugin()
, m_initResources(1)
, m_inputTextureLocationArray(nullptr)
, m_nInputs(nInputs)
{
    SetMinInputs(m_nInputs);
    SetMaxInputs(m_nInputs);
    
    if (m_nInputs > 0) {
        m_inputTextureLocationArray = (GLint*)malloc(sizeof(GLint) * m_nInputs);
        for (int ii = 0; ii < m_nInputs; ii++) {
            m_inputTextureLocationArray[ii] = -1;
        }
    }
        
    SetTimeSupported(true);
    m_HostSupportsSetTime = false;
    
    m_time = 0;
    m_timeLocation = -1;
    
    m_resolutionX = 0;
    m_resolutionY = 0;
    m_resolutionXLocation = -1;
    m_resolutionYLocation = -1;

    InitParameters();

    for (int ii = 0; ii < m_parameters.size(); ii++) {
        auto p = m_parameters[ii];
        SetParamInfo(ii, p.Name.c_str(), p.Type, p.Value);
    }
}

SourcePlugin::SourcePlugin()
: ShaderPlugin(0)
{
}
int SourcePlugin::Type = FF_SOURCE;


EffectPlugin::EffectPlugin()
: ShaderPlugin(1)
{
}
int EffectPlugin::Type = FF_EFFECT;

void ShaderPlugin::InitParameters()
{
    // Parameters from user shader macro block
    m_parameters.assign(shaderParameters.begin(), shaderParameters.end());

    // Built-in parameters provided by the framework
    m_parameters.push_back(Parameter("Randomize", 0.0, 1.0, 0.0, FF_TYPE_EVENT, false,
        [](Parameter& randomp, float newValue, ParamList& list) {
            if (newValue != 1.0) return;
            for (auto& p : list) {
                if (p.Name != randomp.Name) {
                    if (p.Type == FF_TYPE_BOOLEAN) {
                        p.Value = dice() >= 0.5;
                    } else {
                        p.Value = dice();
                    }
                }
            }
        }
    ));
}

DWORD ShaderPlugin::InitGL(const FFGLViewportStruct *vp)
{
    m_extensions.Initialize();
    if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
        return FF_FAIL;
    
    m_shader.SetExtensions(&m_extensions);
    m_shader.Compile(vertexShaderCode, fragmentShaderCode);
 
    m_shader.BindShader();
    
    for (auto& p : m_parameters) {
        if (!p.IsShaderUniform)
            continue;
        p.UniformLocation = m_shader.FindUniform(p.Name.c_str());
        if (p.UniformLocation < 0) {
            fprintf(stderr, "Could not locate uniform %s in shader!\n", p.Name.c_str());
        }
    }
    
    for (int ii = 0; ii < m_nInputs; ii++) {
        stringstream uniformName;
        uniformName << "inputTexture" << ii;
        m_inputTextureLocationArray[ii] = m_shader.FindUniform(uniformName.str().c_str());
        m_extensions.glUniform1iARB(m_inputTextureLocationArray[ii], 0);
    }
        
    m_timeLocation = m_shader.FindUniform("iGlobalTime");
    m_resolutionXLocation = m_shader.FindUniform("iResolutionX");
    m_resolutionYLocation = m_shader.FindUniform("iResolutionY");
    m_resolutionX = vp->width;
    m_resolutionY = vp->height;
    m_aspectRatio = m_resolutionX / m_resolutionY;
    
    m_shader.UnbindShader();
       
    return FF_SUCCESS;
}

DWORD ShaderPlugin::DeInitGL()
{
  m_shader.FreeGLResources();

  return FF_SUCCESS;
}

DWORD ShaderPlugin::ProcessOpenGL(ProcessOpenGLStruct *pGL) {
    
    if (pGL->numInputTextures < m_nInputs)
        return FF_FAIL;
    for (int ii = 0; ii < m_nInputs; ii++) {
        if (pGL->inputTextures[ii] == nullptr)
            return FF_FAIL;
    }
        
    m_shader.BindShader();
    
    m_texDimensions.s = 1;
    m_texDimensions.t = 1;
    
    for (int ii = 0; ii < m_nInputs; ii++) {
        FFGLTextureStruct &Texture = *(pGL->inputTextures[ii]);
        m_extensions.glActiveTexture(GL_TEXTURE0 + ii);
        glBindTexture(GL_TEXTURE_2D, Texture.Handle);
        m_texDimensions = GetMaxGLTexCoords(Texture);
        m_aspectRatio = Texture.Width / Texture.Height;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    for (auto& p : m_parameters) {
        if (!p.IsShaderUniform)
            continue;
        m_extensions.glUniform1fARB(p.UniformLocation, p.GetScaledValue());
    }
    
    if (!m_HostSupportsSetTime)
    {
        update_time(&m_time, m_startTime);
    }
    
    m_extensions.glUniform1fARB(m_timeLocation, m_time);
    m_extensions.glUniform1fARB(m_resolutionXLocation, m_resolutionX);
    m_extensions.glUniform1fARB(m_resolutionYLocation, m_resolutionY);
    
    EmitGeometry();
  
    for (int ii = 0; ii < m_nInputs; ii++) {
        m_extensions.glActiveTexture(GL_TEXTURE0 + ii);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_shader.UnbindShader();
        
    return FF_SUCCESS;
}

void ShaderPlugin::EmitGeometry()
{
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
    glTexCoord2f(0, m_texDimensions.t);
	glVertex2f(-1, 1);
    glTexCoord2f(m_texDimensions.s, m_texDimensions.t);
	glVertex2f(1, 1);
    glTexCoord2f(m_texDimensions.s, 0);
	glVertex2f(1, -1);
	glEnd();
}

DWORD ShaderPlugin::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

    if (dwIndex < m_parameters.size()) {
        auto p = m_parameters[dwIndex];
        *((float *)(unsigned)&dwRet) = p.Value;
        return dwRet;
    } else {
        return FF_FAIL;
	}
}

DWORD ShaderPlugin::SetParameter(const SetParameterStruct* pParam)
{
    if (pParam != NULL && pParam->ParameterNumber < m_parameters.size()) {
        auto& p = m_parameters[pParam->ParameterNumber];
        float newValue = *((float *)(unsigned)&(pParam->NewParameterValue));
        if (p.Action != nullptr)
            p.Action(p, newValue, m_parameters);
        else
            p.Value = newValue;
        return FF_SUCCESS;
    } else {
        return FF_FAIL;
    }
}

DWORD ShaderPlugin::SetTime(double time)
{
    m_HostSupportsSetTime = true;
    m_time = time;
    return FF_SUCCESS;
}

void update_time(double *t, const double t0)
{
#ifdef _WIN32
    //amount of time since plugin init, in seconds (same as SetTime):
    *t = double(GetTickCount())/1000.0 - t0;
#else
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    *t = double(millis)/1000.0f - t0;
#endif
    return;
}

