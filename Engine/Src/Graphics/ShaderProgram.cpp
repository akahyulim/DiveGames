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

		const unsigned int* pVsBufferSizes = pVS->GetConstantBufferSizes();
		for (unsigned int i = 0; i < 7; ++i)
		{
			if (pVsBufferSizes[i])
				m_pVSConstantBuffers[i] = pGraphics->GetOrCreateConstantBuffer(eShaderType::Vertex, i, pVsBufferSizes[i]);
		}

		const std::unordered_map<std::string, ShaderParameter>& vsParams = pVS->GetParameters();
		for (auto it = vsParams.begin(); it != vsParams.end(); ++it)
		{
			m_Parameters[it->first] = it->second;
			m_Parameters[it->first].m_pBuffer = m_pVSConstantBuffers[it->second.m_Index];
		}

		const unsigned int* pPsBufferSizes = pPS->GetConstantBufferSizes();
		for (unsigned int i = 0; i < 7; ++i)
		{
			if (pPsBufferSizes[i])
				m_pPSConstantBuffers[i] = pGraphics->GetOrCreateConstantBuffer(eShaderType::Pixel, i, pPsBufferSizes[i]);
		}

		const std::unordered_map<std::string, ShaderParameter>& psParams = pPS->GetParameters();
		for (auto it = psParams.begin(); it != psParams.end(); ++it)
		{
			m_Parameters[it->first] = it->second;
			m_Parameters[it->first].m_pBuffer = m_pPSConstantBuffers[it->second.m_Index];
		}

	}

	ShaderProgram::~ShaderProgram()
	{
		DV_LOG_ENGINE_DEBUG("ShaderProgram �Ҹ��� ȣ��");

		// �� ���� ��ϵ� ���۴� �ٸ� ���̴������� ����� �� �ֱ⿡
		// ���� �����ϸ� �ȵȴ�.
		// Graphics�� ��� ConstantBuffer�� �����ϴ� �� ���� ���� �� �ϴ�.
	}
}