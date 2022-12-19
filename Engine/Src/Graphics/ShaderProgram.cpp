#include "divepch.h"
#include "ShaderVariation.h"
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "Math/StringHash.h"
#include "IO/Log.h"

namespace Dive
{
	ShaderProgram::ShaderProgram(Graphics* pGraphics, ShaderVariation* pVS, ShaderVariation* pPS)
	{
		if (!pGraphics || !pGraphics->IsInitialized())
			return;

		const uint32_t* pVsBufferSizes = pVS->GetConstantBufferSizes();
		for (uint32_t i = 0; i < 7; ++i)
		{
			if (pVsBufferSizes[i])
				m_pVSConstantBuffers[i] = pGraphics->GetOrCreateConstantBuffer(eShaderType::Vertex, i, pVsBufferSizes[i]);
		}

		const std::unordered_map<std::string, ShaderParameter>& vsParams = pVS->GetParameters();
		for (auto it = vsParams.begin(); it != vsParams.end(); ++it)
		{
			m_Parameters[it->first] = it->second;
			m_Parameters[it->first].pBuffer = m_pVSConstantBuffers[it->second.Index];
		}

		const uint32_t* pPsBufferSizes = pPS->GetConstantBufferSizes();
		for (uint32_t i = 0; i < 7; ++i)
		{
			if (pPsBufferSizes[i])
				m_pPSConstantBuffers[i] = pGraphics->GetOrCreateConstantBuffer(eShaderType::Pixel, i, pPsBufferSizes[i]);
		}

		const std::unordered_map<std::string, ShaderParameter>& psParams = pPS->GetParameters();
		for (auto it = psParams.begin(); it != psParams.end(); ++it)
		{
			m_Parameters[it->first] = it->second;
			m_Parameters[it->first].pBuffer = m_pPSConstantBuffers[it->second.Index];
		}

	}

	ShaderProgram::~ShaderProgram()
	{
		DV_LOG_ENGINE_TRACE("ShaderProgram 소멸 완료");

		// 이 곳에 등록된 버퍼는 다른 셰이더에서도 사용할 수 있기에
		// 직접 해제하면 안된다.
		// Graphics에 모든 ConstantBuffer가 존재하니 그 곳이 맞을 듯 하다.
	}
}