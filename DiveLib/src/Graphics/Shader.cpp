#include "Graphics/Shader.h"
#include "Graphics/Graphics.h"
#include "Core/Common.h"
#include "Core/LogManager.h"
#include <assert.h>

namespace Dive
{
	VertexShader::VertexShader(Graphics* graphics, ID3DBlob* code)
	{
		auto device = graphics->GetDevice();
		auto hr = device->CreateVertexShader(
			code->GetBufferPointer(),
			code->GetBufferSize(),
			nullptr,
			m_vs.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create VertexShader: {}", ErrorUtils::ToVerbose(hr));
			assert(false && "VertexShader 생성 실패!");
		}
	}

	VertexShader::~VertexShader() = default;


	HullShader::HullShader(Graphics* graphics, ID3DBlob* code)
	{
		auto device = graphics->GetDevice();
		auto hr = device->CreateHullShader(
			code->GetBufferPointer(),
			code->GetBufferSize(),
			nullptr,
			m_hs.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create HullShader: {}", ErrorUtils::ToVerbose(hr));
			assert(false && "HullShader 생성 실패!");
		}
	}

	HullShader::~HullShader() = default;

	DomainShader::DomainShader(Graphics* graphics, ID3DBlob* code)
	{
		auto device = graphics->GetDevice();
		auto hr = device->CreateDomainShader(
			code->GetBufferPointer(),
			code->GetBufferSize(),
			nullptr,
			m_ds.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create DomainShader: {}", ErrorUtils::ToVerbose(hr));
			assert(false && "DomainShader 생성 실패!");
		}
	}

	DomainShader::~DomainShader() = default;

	PixelShader::PixelShader(Graphics* graphics, ID3DBlob* code)
	{
		auto device = graphics->GetDevice();
		auto hr = device->CreatePixelShader(
			code->GetBufferPointer(),
			code->GetBufferSize(),
			nullptr,
			m_ps.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create PixelShader: {}", ErrorUtils::ToVerbose(hr));
			assert(false && "PixelShader 생성 실패!");
		}
	}

	PixelShader::~PixelShader() = default;

	ComputeShader::ComputeShader(Graphics* graphics, ID3DBlob* code)
	{
		auto device = graphics->GetDevice();
		auto hr = device->CreateComputeShader(
			code->GetBufferPointer(),
			code->GetBufferSize(),
			nullptr,
			m_cs.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "Failed to create ComputeShader: {}", ErrorUtils::ToVerbose(hr));
			assert(false && "ComputeShader 생성 실패!");
		}
	}

	ComputeShader::~ComputeShader() = default;
}
