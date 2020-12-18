#include "DivePch.h"
#include "InputLayout.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Graphics/Graphics.h"


namespace Dive
{
	InputLayout::InputLayout(Context * context)
		: Object(context),
		m_buffer(nullptr)
	{
	}

	InputLayout::~InputLayout()
	{
		if(!m_descs.empty())
			m_descs.clear();
		SAFE_RELEASE(m_buffer);
	}

	bool InputLayout::createBuffer(ID3DBlob * blob)
	{
		if (!blob)
		{
			CORE_ERROR("");
			return false;
		}

		auto graphics = GetSubsystem<Graphics>();
		if (FAILED(graphics->GetRHIDevice()->CreateInputLayout(
			m_descs.data(),
			static_cast<unsigned int>(m_descs.size()),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&m_buffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
	
	void InputLayout::createPositionDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "POSITION";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= 0;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_descs.emplace_back(desc);
	}
	
	void InputLayout::createColorDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "COLOR";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_descs.emplace_back(desc);
	}

	void InputLayout::createTextureDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "TEXCOORD";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_descs.emplace_back(desc);
	}

	void InputLayout::createNormalDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "NORMAL";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_descs.emplace_back(desc);
	}

	void InputLayout::createTangentDesc()
	{
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName			= "TANGENT";
		desc.SemanticIndex			= 0;
		desc.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		desc.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot				= 0;
		desc.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate	= 0;

		m_descs.emplace_back(desc);
	}

	void InputLayout::create2dDesc()
	{
		D3D11_INPUT_ELEMENT_DESC descPosition;
		descPosition.SemanticName			= "POSITION";
		descPosition.SemanticIndex			= 0;
		descPosition.AlignedByteOffset		= 0;
		descPosition.Format					= DXGI_FORMAT_R32G32_FLOAT;
		descPosition.InputSlot				= 0;
		descPosition.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		descPosition.InstanceDataStepRate	= 0;

		m_descs.emplace_back(descPosition);

		D3D11_INPUT_ELEMENT_DESC descTexture;
		descTexture.SemanticName			= "TEXCOORD";
		descTexture.SemanticIndex			= 0;
		descTexture.AlignedByteOffset		= D3D11_APPEND_ALIGNED_ELEMENT;
		descTexture.Format					= DXGI_FORMAT_R32G32_FLOAT;
		descTexture.InputSlot				= 0;
		descTexture.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		descTexture.InstanceDataStepRate	= 0;

		m_descs.emplace_back(descTexture);

		D3D11_INPUT_ELEMENT_DESC descColor;
		descColor.SemanticName				= "COLOR";
		descColor.SemanticIndex				= 0;
		descColor.AlignedByteOffset			= D3D11_APPEND_ALIGNED_ELEMENT;
		descColor.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		descColor.InputSlot					= 0;
		descColor.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		descColor.InstanceDataStepRate		= 0;

		m_descs.emplace_back(descColor);
	}
}