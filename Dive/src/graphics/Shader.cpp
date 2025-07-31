#include "stdafx.h"
#include "Shader.h"
#include "Graphics.h"
#include "core/FileUtils.h"
#include "core/StringUtils.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	VertexShader::~VertexShader()
	{
		m_blob.Reset();
		m_shader.Reset();
	}

	bool VertexShader::LoadFromFile(const std::filesystem::path& path, ID3D11Device* device)
	{
		assert(device);

		ComPtr<ID3DBlob> errorBlob;

		auto hr = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"MainVS",
			"vs_5_0",
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			m_blob.GetAddressOf(),
			errorBlob.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(VertexShader, err, "[::LoadFromFile] D3DCompileFromFile 실패: {}",
				errorBlob ? static_cast<char*>(errorBlob->GetBufferPointer()) : ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = device->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, m_shader.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(VertexShader, err, "[::LoadFromFile] CreateVertexShader 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		SetName(path.stem().string() + "_VS");

		return true;
	}

	void VertexShader::Bind(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);
		deviceContext->VSSetShader(m_shader.Get(), nullptr, 0);
	}

	PixelShader::~PixelShader()
	{
		m_shader.Reset();
	}

	bool PixelShader::LoadFromFile(const std::filesystem::path& path, ID3D11Device* device)
	{
		assert(device);

		ComPtr<ID3DBlob> compiledBlob;
		ComPtr<ID3DBlob> errorBlob;

		auto hr = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"MainPS",
			"ps_5_0",
			D3D10_SHADER_ENABLE_STRICTNESS,
			0,
			compiledBlob.GetAddressOf(),
			errorBlob.GetAddressOf());

		if (FAILED(hr)) {
			DV_LOG(PixelShader, err, "[::LoadFromFile] D3DCompileFromFile 실패: {}",
				errorBlob ? static_cast<char*>(errorBlob->GetBufferPointer()) : ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = device->CreatePixelShader(
			compiledBlob->GetBufferPointer(),
			compiledBlob->GetBufferSize(),
			nullptr,
			m_shader.GetAddressOf());

		if (FAILED(hr)) {
			DV_LOG(PixelShader, err, "[::LoadFromFile] CreatePixelShader 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		SetName(path.stem().string() + "_PS");

		return true;
	}

	void PixelShader::Bind(ID3D11DeviceContext* deviceContext)
	{
		assert(deviceContext);
		deviceContext->PSSetShader(m_shader.Get(), nullptr, 0);
	}
}
