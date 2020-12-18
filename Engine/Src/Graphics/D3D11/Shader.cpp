#include "DivePch.h"
#include "Shader.h"


namespace Dive
{
	Shader::Shader(Context * context)
		: Object(context),
		m_vertexShader(nullptr),
		m_pixelShader(nullptr)
	{
	}

	Shader::~Shader()
	{
	}
}