#include "DivePch.h"
#include "Shader.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Graphics/Graphics.h"


namespace Dive
{
	Shader::Shader(Context * context)
		: Object(context),
		m_vertexShader(nullptr),
		m_pixelShader(nullptr),
		m_inputLayout(nullptr),
		m_vertexType(eVertexType::Unknown),
		m_shaderType(eShaderType::Unknown)
	{
	}

	Shader::~Shader()
	{
		if (!m_inputDescs.empty())
		{
			m_inputDescs.clear();
		}

		SAFE_RELEASE(m_inputLayout);
		SAFE_RELEASE(m_pixelShader);
		SAFE_RELEASE(m_vertexShader);
	}

	bool Shader::CreateShader(eVertexType vertexType, std::wstring filepath)
	{
		if (!CreateVertexShader(vertexType, filepath))
			return false;

		if (!CreatePixelShader(filepath))
			return false;

		m_shaderType = eShaderType::VertexPixel;

		return true;
	}
	
	bool Shader::CreateVertexShader(eVertexType vertexType, std::wstring filepath)
	{
		m_vertexType = vertexType;
		m_shaderType = eShaderType::Vertex;

		if (!compileAndCreateShader(filepath))
			return false;

		CORE_TRACE("");
		return true;
	}
	
	bool Shader::CreatePixelShader(std::wstring filepath)
	{
		m_shaderType = eShaderType::Pixel;

		if (!compileAndCreateShader(filepath))
			return false;

		CORE_TRACE("");
		return true;
	}

	bool Shader::compileAndCreateShader(std::wstring filepath)
	{
		// 지원 파일 및 포멧인지 확인 후
		// 이름도 저장
		{
			m_filepath = filepath;
		}

		ID3DBlob* blobShader	= nullptr;
		ID3DBlob* blobError		= nullptr;

		// compile
		{
			// macro...

			unsigned int compileFlags = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if defined(DEBUG) || defined(_DEBUG)
			compileFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

			auto result = D3DCompileFromFile(
				m_filepath.c_str(),
				nullptr, // macro
				D3D_COMPILE_STANDARD_FILE_INCLUDE,
				m_shaderType == eShaderType::Vertex ? "mainVS" : "mainPS",
				m_shaderType == eShaderType::Vertex ? "vs_5_0" : "ps_5_0",
				compileFlags,
				0,
				&blobShader,
				&blobError);

			if (blobError)
			{
				std::stringstream ss(static_cast<char*>(blobError->GetBufferPointer()));
				std::string line;
				while (std::getline(ss, line, '\n'))
				{
					const auto is_error = line.find("error") != std::string::npos;
					if (is_error)
						CORE_ERROR("{:s}", line);
					else
						CORE_WARN("{:s}", line);
				}

				SAFE_RELEASE(blobError);
				return false;
			}

			if (FAILED(result) || !blobShader)
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create
		{
			switch (m_shaderType)
			{
			case eShaderType::Vertex:
			{
				if (!createVertexShader(blobShader))
					return false;

				if (!createInputLayout(blobShader))
					return false;

				break;
			}
			case eShaderType::Pixel:
			{
				if (!createPixelShader(blobShader))
					return false;

				break;
			}

			default:
			{
				SAFE_RELEASE(blobShader);
				CORE_ERROR("");
				return false;
			}
			}
		}

		SAFE_RELEASE(blobShader);
		
		return true;
	}

	bool Shader::createVertexShader(ID3D10Blob * blob)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!blob || !graphics->IsInitialized())
			return false;

		if (FAILED(graphics->GetRHIDevice()->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			&m_vertexShader)))
		{
			CORE_ERROR("");
			SAFE_RELEASE(blob);
			return false;
		}

		return true;
	}

	bool Shader::createPixelShader(ID3D10Blob * blob)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!blob || !graphics->IsInitialized())
			return false;

		if (FAILED(graphics->GetRHIDevice()->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			&m_pixelShader)))
		{
			CORE_ERROR("");
			SAFE_RELEASE(blob);
			return false;
		}

		return true;
	}

	bool Shader::createInputLayout(ID3D10Blob* blob)
	{
		switch (m_vertexType)
		{
		case eVertexType::Position:
			createPositionDesc();
			break;

		case eVertexType::PositionColor:
			createPositionDesc();
			createColorDesc();
			break;

		case eVertexType::PositionUv:
			createPositionDesc();
			createTextureDesc();
			break;

		case eVertexType::PositionUvNormalTangent:
			createPositionDesc();
			createTextureDesc();
			createNormalDesc();
			createTangentDesc();
			break;

		default:
			CORE_ERROR("");
			SAFE_RELEASE(blob);
			return false;
		}

		auto graphics = GetSubsystem<Graphics>();
		if (!blob || !graphics->IsInitialized())
			return false;

		if (FAILED(graphics->GetRHIDevice()->CreateInputLayout(
			m_inputDescs.data(),
			static_cast<unsigned int>(m_inputDescs.size()),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&m_inputLayout)))
		{
			CORE_ERROR("");
			SAFE_RELEASE(blob);
			CORE_ERROR("");
			return false;
		}

		m_inputDescs.clear();
		return true;
	}
	
	void Shader::createPositionDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "POSITION";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= 0;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_inputDescs.emplace_back(desc);
	}

	void Shader::createColorDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "COLOR";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_inputDescs.emplace_back(desc);
	}

	void Shader::createTextureDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "TEXCOORD";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_inputDescs.emplace_back(desc);
	}

	void Shader::createNormalDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "NORMAL";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_inputDescs.emplace_back(desc);
	}

	void Shader::createTangentDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "TANGENT";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_inputDescs.emplace_back(desc);
	}
}