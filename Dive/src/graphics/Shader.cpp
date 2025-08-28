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

	bool VertexShader::LoadFromFile(const std::filesystem::path& path)
	{
		ComPtr<ID3DBlob> errorBlob;

		auto hr = D3DReadFileToBlob(path.c_str(), m_blob.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(VertexShader, err, "[::LoadFromFile] cso 파일 읽기 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = Graphics::GetDevice()->CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, m_shader.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(VertexShader, err, "[::LoadFromFile] CreateVertexShader 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		SetName(path.stem().string());

		return true;
	}

	void VertexShader::Bind()
	{
		Graphics::GetDeviceContext()->VSSetShader(m_shader.Get(), nullptr, 0);
	}

	PixelShader::~PixelShader()
	{
		m_shader.Reset();
	}

	bool PixelShader::LoadFromFile(const std::filesystem::path& path)
	{
		ComPtr<ID3DBlob> compiledBlob;
		ComPtr<ID3DBlob> errorBlob;

		auto hr = D3DReadFileToBlob(path.c_str(), compiledBlob.GetAddressOf());
		if (FAILED(hr))
		{
			DV_LOG(PixelShader, err, "[::LoadFromFile] cso 파일 읽기 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = Graphics::GetDevice()->CreatePixelShader(
			compiledBlob->GetBufferPointer(),
			compiledBlob->GetBufferSize(),
			nullptr,
			m_shader.GetAddressOf());

		if (FAILED(hr)) {
			DV_LOG(PixelShader, err, "[::LoadFromFile] CreatePixelShader 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		SetName(path.stem().string());

		return true;
	}

	void PixelShader::Bind()
	{
		Graphics::GetDeviceContext()->PSSetShader(m_shader.Get(), nullptr, 0);
	}
}
