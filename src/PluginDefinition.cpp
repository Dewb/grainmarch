#include <FFGL.h>

#include "PluginDefinition.h"

#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

#ifndef WIN32
#include <sys/time.h>
#endif

char DefaultTextValue[] = "";

float dice() {
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

// Action to convert a cycling parameter into an accumulated linear ramp
ParamAction AccumulateAction = [](Parameter& p, float v, ParamList& l) {
    if (p.ActionData[0] - v > 0.5) {
        p.ActionData[1] += 1.0;
    } else if (v - p.ActionData[0] > 0.5) {
        p.ActionData[1] -= 1.0;
    }
    p.ActionData[0] = v;
    p.Value = p.ActionData[1] + v;
};

Parameter::Parameter(string name, float min, float max, float value, int type, bool isShader, bool shouldRandomize, ParamAction action, char* textValue)
: Name(name)
, Type(type)
, RangeMin(min)
, RangeMax(max)
, IsShaderUniform(isShader)
, ShouldRandomize(shouldRandomize)
, UniformLocation(-1)
, Action(action)
{
    if (type == FF_TYPE_TEXT) {
        TextValue = (textValue != nullptr) ? textValue : DefaultTextValue;
    } else {
        Value = (value - min) / (max - min);
    }
}

float Parameter::GetScaledValue() const {
    return RangeMin + Value * (RangeMax - RangeMin);
}

const char* Parameter::GetTextValue() const {
	return TextValue.c_str();
}

void Parameter::SetTextValue(const char* value) {
	TextValue = value;
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
    
    m_resolutionX = 0;
    m_resolutionY = 0;
    m_resolutionXLocation = -1;
    m_resolutionYLocation = -1;

    InitParameters();

    for (int ii = 0; ii < m_parameters.size(); ii++) {
        auto p = m_parameters[ii];
        if (p.Type == FF_TYPE_TEXT) {
            SetParamInfo(ii, p.Name.c_str(), p.Type, p.GetTextValue());
        } else {
            SetParamInfo(ii, p.Name.c_str(), p.Type, p.Value);
        }
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
    m_parameters.assign(Param::macroParameters.begin(), Param::macroParameters.end());

    // Built-in parameters provided by the framework
    m_parameters.push_back(Parameter("Randomize", 0.0, 1.0, 0.0, FF_TYPE_EVENT, false, false,
        [](Parameter& randomp, float newValue, ParamList& list) {
            if (newValue != 1.0) return;
            for (auto& p : list) {
                if (p.ShouldRandomize) {
                    if (p.Type == FF_TYPE_BOOLEAN) {
                        p.Value = dice() >= 0.5;
                    } else {
                        p.Value = dice();
                    }
                }
            }
        }
    ));
    m_parameters.push_back(Parameter("RandomHold", 0.0, 1.0, 0.0, FF_TYPE_BOOLEAN, false, false,
         [](Parameter& randomp, float newValue, ParamList& list) {
             static vector<float> diceRolls;
             
             if (newValue == 1.0 && randomp.Value != 1.0) {
                 diceRolls.clear();
                 for (auto& p : list) {
                     if (p.ShouldRandomize) {
                         float r;
                         if (p.Type == FF_TYPE_BOOLEAN) {
                             r = dice() >= 0.5;
                         } else {
                             r = dice();
                         }
                         diceRolls.push_back(r);
                         p.LastNonRandomValue = p.Value;
                         p.Value = r;
                     }
                 }
             } else if (newValue == 1.0) {
                 auto roll = diceRolls.begin();
                 for (auto& p : list) {
                     if (p.ShouldRandomize) {
                         p.Value = p.Value - p.LastNonRandomValue + *roll++;
                     }
                 }
             }
             randomp.Value = newValue;
         }
    ));
}

FFResult ShaderPlugin::InitGL(const FFGLViewportStruct *vp)
{
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
        glUniform1i(m_inputTextureLocationArray[ii], ii);
    }
        
    m_resolutionXLocation = m_shader.FindUniform("iResolutionX");
    m_resolutionYLocation = m_shader.FindUniform("iResolutionY");
    m_resolutionX = vp->width;
    m_resolutionY = vp->height;
    m_aspectRatio = m_resolutionX / m_resolutionY;
    
    Initialize();
    
    m_shader.UnbindShader();
    
    return FF_SUCCESS;
}

FFResult ShaderPlugin::DeInitGL()
{
  m_shader.FreeGLResources();

  return FF_SUCCESS;
}

FFResult ShaderPlugin::ProcessOpenGL(ProcessOpenGLStruct *pGL) {
    
    if (pGL->numInputTextures < m_nInputs)
        return FF_FAIL;
    for (int ii = 0; ii < m_nInputs; ii++) {
        if (pGL->inputTextures[ii] == nullptr)
            return FF_FAIL;
    }

	float viewportParams[4];
	glGetFloatv(GL_VIEWPORT, viewportParams);
	m_resolutionX = viewportParams[2];
	m_resolutionY = viewportParams[3];
	
    UpdateUniforms();
        
    m_shader.BindShader();
    
    m_texDimensions.s = 1;
    m_texDimensions.t = 1;

    for (int ii = 0; ii < m_nInputs; ii++) {
        FFGLTextureStruct &Texture = *(pGL->inputTextures[ii]);

		glActiveTexture(GL_TEXTURE0 + ii);
        glBindTexture(GL_TEXTURE_2D, Texture.Handle);
        m_texDimensions = GetMaxGLTexCoords(Texture);
        m_aspectRatio = Texture.Width / Texture.Height;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    for (auto& p : m_parameters) {
        if (!p.IsShaderUniform)
            continue;
        glUniform1f(p.UniformLocation, p.GetScaledValue());
    }
    
    for (auto& u : m_floatUniforms) {
        glUniform1f(std::get<0>(u), *(std::get<2>(u)));
    }

    for (auto& u : m_floatArrayUniforms) {
        glUniform1fv(std::get<0>(u), std::get<1>(u), std::get<2>(u));
    }

    glUniform1f(m_resolutionXLocation, m_resolutionX);
    glUniform1f(m_resolutionYLocation, m_resolutionY);
    
    EmitGeometry();
  
    for (int ii = 0; ii < m_nInputs; ii++) {
        glActiveTexture(GL_TEXTURE0 + ii);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_shader.UnbindShader();
        
    return FF_SUCCESS;
}


void ShaderPlugin::Initialize()
{
}

void ShaderPlugin::EmitGeometry()
{
    glBegin(GL_QUADS);
	glMultiTexCoord2f(GL_TEXTURE0, 0, 0);
	glVertex2f(-1, -1);
	glMultiTexCoord2f(GL_TEXTURE0, 0, m_texDimensions.t);
	glVertex2f(-1, 1);
	glMultiTexCoord2f(GL_TEXTURE0, m_texDimensions.s, m_texDimensions.t);
	glVertex2f(1, 1);
	glMultiTexCoord2f(GL_TEXTURE0, m_texDimensions.s, 0);
	glVertex2f(1, -1);
	glEnd();
}

void ShaderPlugin::UpdateUniforms()
{
}

float ShaderPlugin::GetFloatParameter(unsigned int index)
{
    if (index < m_parameters.size()) {
        auto p = m_parameters[index];
		return p.Value;
    } else {
        return FF_FAIL;
	}
}

FFResult ShaderPlugin::SetFloatParameter(unsigned int index, float value)
{
    if (index < m_parameters.size()) {
        auto& p = m_parameters[index];
        if (p.Action != nullptr) {
            p.Action(p, value, m_parameters);
        } else {
            p.Value = value;
        }
        return FF_SUCCESS;
    } else {
        return FF_FAIL;
    }
}

FFResult ShaderPlugin::SetTextParameter(unsigned int index, const char *value)
{
	if (index < m_parameters.size()) {
		auto& p = m_parameters[index];
		p.SetTextValue(value);
	} else {
		return FF_FAIL;
	}
}

char* ShaderPlugin::GetTextParameter(unsigned int index)
{
	if (index < m_parameters.size()) {
		auto& p = m_parameters[index];
		return const_cast<char*>(p.GetTextValue());
	} else {
		return nullptr;
	}
}

FFResult ShaderPlugin::SetTime(double time)
{
    return FF_SUCCESS;
}

void ShaderPlugin::ManuallyBindUniformFloat(string Name, float *pValue)
{
    GLint location = m_shader.FindUniform(Name.c_str());
    m_floatUniforms.push_back(make_tuple(location, 1.0, pValue));
}

void ShaderPlugin::ManuallyBindUniformFloatArray(string Name, float count, float *pValue)
{
    GLint location = m_shader.FindUniform(Name.c_str());
    m_floatArrayUniforms.push_back(make_tuple(location, count, pValue));
}