#include "Rendering/Renderer2D.h"
#include "Core/LogManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Geometry.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Shader.h"

namespace Dive
{
	namespace
	{
		constexpr uint32_t MaxQuads = 20000;
		constexpr uint32_t MaxVertices = MaxQuads * 4;
		constexpr uint32_t MaxIndices = MaxQuads * 6;
		constexpr uint32_t MaxTextureSlots = 32;
	}

	Renderer2D::Renderer2D(Graphics* graphics)
		: m_graphics(graphics)
		, m_width(graphics->GetWidth())
		, m_height(graphics->GetHeight())
	{
		{
			// lines
			{
				m_lineVertexBuffer = std::make_unique<VertexBuffer>(
					graphics,
					sizeof(DebugVertex),
					MaxVertices,
					true
				);
				m_lineVertices = std::make_unique<DebugVertex[]>(MaxVertices);
				m_lineVertexCount = 0;
			}

			// quads
			{
				m_quadVertexBuffer = std::make_unique<VertexBuffer>(
					graphics,
					sizeof(SpriteVertex),
					MaxVertices,
					true
				);
				m_quadVertices = std::make_unique<SpriteVertex[]>(MaxVertices);
				m_quadVertexCount = 0;

				std::vector<uint32_t> quadIndices;
				quadIndices.reserve(MaxIndices);

				uint32_t offset = 0;
				for (uint32_t i = 0; i < MaxQuads; ++i)
				{
					quadIndices.push_back(offset + 0);
					quadIndices.push_back(offset + 1);
					quadIndices.push_back(offset + 2);

					quadIndices.push_back(offset + 0);
					quadIndices.push_back(offset + 2);
					quadIndices.push_back(offset + 3);

					offset += 4;
				}

				m_quadIndexBuffer = std::make_unique<IndexBuffer>(
					graphics,
					eFormat::R32_UINT,
					MaxIndices,
					quadIndices.data()
				);
			}

			m_cbFrame = std::make_unique<Dive::ConstantBuffer<FrameData>>(graphics);
			if (m_cbFrame == nullptr)
			{
				DV_LOG(Renderer, err, "Fail to create a FrameData constant buffer.");
			}
		}

		ShaderManager::Get().Initialize2D(graphics);
		
		createDepthStencilStates();
		createRasterizerStates();
		createBlendStates();
		createSamplers();
		createResolutionDependantResources(graphics->GetWidth(), graphics->GetHeight());

		// Set/Clear RenderTarget이 없어 기존 pass를 사용했다.
		// 추후 Set/Clear를 추가해야 한다.
		{
			Dive::RenderTargetDesc rtDesc{};
			rtDesc.RenderTargetView = m_screenRenderTarget->GetRenderTargetView();	// 생성 후 연결해야 한다.
			rtDesc.ClearColor[0] = 0.0f;
			rtDesc.ClearColor[1] = 0.0f;
			rtDesc.ClearColor[2] = 0.0f;
			rtDesc.ClearColor[3] = 1.0f;
			rtDesc.AccessType = Dive::eLoadAccessOp::Clear;

			Dive::RenderPassDesc desc{};
			desc.renderTargetDescs.push_back(rtDesc);
			m_pass = desc;
		}

		DV_LOG(Renderer, info, "Renderer system initialization completed successfully.");
	}

	Renderer2D::~Renderer2D()
	{
		DV_LOG(Renderer, info, "Renderer system shut down successfully.");
	}
	
	void Renderer2D::ResolveToBackbuffer()
	{
		m_graphics->SetBackbuffer();

		auto srv = m_screenRenderTarget->GetShaderResourceView();
		m_graphics->SetShaderResourceView(eShaderStage::PS, 1, &srv);
		ShaderManager::Get().GetShaderProgram("ResolveScreen")->Bind(m_graphics);	// enum class로 하지 않았다. 추후 추가할수도..
		m_graphics->SetTopology(ePrimitiveTopology::TriangleStrip);
		m_graphics->SetVertexBuffer(nullptr);
		m_graphics->Draw(4);
	}

	// 직접 리사이즈
	void Renderer2D::Resize(uint32_t width, uint32_t height)
	{
	}

	// 윈도우 크기 변화에 대응
	void Renderer2D::OnResize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;

		if (m_width != 0 || m_height != 0)
		{
			createResolutionDependantResources(width, height);
		}
	}

	// 기본적으로는 카메라를 전달받지만 일단은 이렇게 사용
	void Renderer2D::BeginScene(const DirectX::XMMATRIX& viewProj)
	{
		FrameData data = {};
		data.viewProj = viewProj;
		m_cbFrame->Update(m_graphics, data);

		// 현재 Set/Clear RenderTarget이 없다.
		m_graphics->BeginRenderPass(m_pass);

		// 이건 사실 한 번만 하면 된다.
		// 셰이더들도 마찬가지?
		{
			auto context = m_graphics->GetDeviceContext();
			ID3D11SamplerState* samplers[static_cast<size_t>(eSamplerState::Count)] =
			{
				m_samplerStates[static_cast<size_t>(eSamplerState::WrapPoint)].Get(),
				m_samplerStates[static_cast<size_t>(eSamplerState::WrapLinear)].Get(),
				m_samplerStates[static_cast<size_t>(eSamplerState::ClampPoint)].Get(),
				m_samplerStates[static_cast<size_t>(eSamplerState::ClampLinear)].Get()
			};

			context->DSSetSamplers(0, static_cast<UINT>(eSamplerState::Count), samplers);
			context->PSSetSamplers(0, static_cast<UINT>(eSamplerState::Count), samplers);
		}

		startBatch();
	}

	void Renderer2D::EndScene()
	{
		flush();

		m_graphics->EndRenderPass();
	}

	void Renderer2D::DrawLine(const DirectX::XMFLOAT2& from, const DirectX::XMFLOAT2& to, const Color& color)
	{
		DrawLine({ from.x, from.y, 0.0f }, { to.x, to.y, 0.0f }, color);
	}

	void Renderer2D::DrawLine(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& to, const Color& color)
	{
		if (m_lineVertexCount + 2 > MaxVertices)
			nextBatch();

		m_lineVertices[m_lineVertexCount].Position = from;
		m_lineVertices[m_lineVertexCount].Color = color;
		m_lineVertexCount++;

		m_lineVertices[m_lineVertexCount].Position = to;
		m_lineVertices[m_lineVertexCount].Color = color;
		m_lineVertexCount++;
	}

	void Renderer2D::DrawTriangle(const DirectX::XMFLOAT2& v0, const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2, const Color& color)
	{
		DrawTriangle({ v0.x, v0.y, 0.0f, }, { v1.x, v1.y, 0.0f }, { v2.x, v2.y, 0.0f }, color);
	}

	void Renderer2D::DrawTriangle(const DirectX::XMFLOAT3& v0, const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, const Color& color)
	{
		DrawLine(v0, v1, color);
		DrawLine(v1, v2, color);
		DrawLine(v2, v0, color);
	}

	void Renderer2D::DrawRect(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const Color& color)
	{
		DrawRect({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawRect(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const Color& color)
	{
		DirectX::XMFLOAT3 v0 = { position.x - (size.x * 0.5f), position.y - (size.y * 0.5f), position.z };		// 좌하
		DirectX::XMFLOAT3 v1 = { position.x + (size.x * 0.5f), position.y - (size.y * 0.5f), position.z };		// 우하
		DirectX::XMFLOAT3 v2 = { position.x + (size.x * 0.5f), position.y + (size.y * 0.5f), position.z };		// 우상
		DirectX::XMFLOAT3 v3 = { position.x - (size.x * 0.5f), position.y + (size.y * 0.5f), position.z };		// 좌상

		DrawLine(v0, v1, color);
		DrawLine(v1, v2, color);
		DrawLine(v2, v3, color);
		DrawLine(v3, v0, color);
	}

	void Renderer2D::DrawRect(const DirectX::XMFLOAT4X4& taransform, const Color& color)
	{
		// 디폴트 QuadVertexPositions의 네 정점을 변환 후 DrawLine으로 전달
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, const Color& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, const Color& color)
	{
		const DirectX::XMMATRIX matTransform =
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, matTransform);

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT4X4& transform, const Color& color)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr size_t quadIndexCount = 6;
		constexpr float textureIndex = 0.0f;

		if (m_quadIndexCount > MaxIndices)
			nextBatch();

		// 단위 쿼드 로컬 좌표 (중심 원점 기준 한 변 길이 1)
		static constexpr DirectX::XMFLOAT3 quadVertexPosition[4] = {
			{ -0.5f, -0.5f, 0.0f },
			{ -0.5f,  0.5f, 0.0f },
			{  0.5f,  0.5f, 0.0f },
			{  0.5f, -0.5f, 0.0f }
		};

		static constexpr DirectX::XMFLOAT2 texCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		const DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&transform);
		const DirectX::XMFLOAT2 tiling = { 1.0f, 1.0f };

		// 2. 4개 정점 변환 및 배치 버퍼 기록
		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			const DirectX::XMVECTOR localPos = DirectX::XMLoadFloat3(&quadVertexPosition[i]);
			const DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(localPos, worldMatrix);

			SpriteVertex& vertex = m_quadVertices[m_quadVertexCount++];
			DirectX::XMStoreFloat3(&vertex.Position, worldPos);
			vertex.Color = color;
			vertex.TexCoord = texCoords[i];
			vertex.TexIndex = textureIndex;
			vertex.Tiling = tiling;
		}

		m_quadIndexCount += quadIndexCount;
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& size, TextureHandle texture, const DirectX::XMFLOAT2& tiling, const Color& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tiling, tintColor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& size, TextureHandle texture, const DirectX::XMFLOAT2& tiling, const Color& tintColor)
	{
		const DirectX::XMMATRIX matTransform =
			DirectX::XMMatrixScaling(size.x, size.y, 1.0f) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DirectX::XMFLOAT4X4 transform{};
		DirectX::XMStoreFloat4x4(&transform, matTransform);

		DrawQuad(transform, texture, tiling, tintColor);
	}

	void Renderer2D::DrawQuad(const DirectX::XMFLOAT4X4& taransform, TextureHandle texture, const DirectX::XMFLOAT2& tiling, const Color& tintColor)
	{
		// texture로부터 texId를 획득해야 한다.
		// hazel은 일단 배열에 전부 넣어놓았고 여기에서 비교한 후 index를 찾아낸다.
	}

	void Renderer2D::createDepthStencilStates()
	{
	}

	void Renderer2D::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc{};

		// 1. Solid + Cull None (2D 스프라이트/도형 기본)
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		m_rasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)] =
		     m_graphics->CreateRasterizerState(desc);

		// 2. Wireframe + Cull None (디버그 확인용)
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		m_rasterizerStates[static_cast<size_t>(eRasterizerState::Wireframe_CullNone)] =
		     m_graphics->CreateRasterizerState(desc);

		// 3. Solid + Cull None + Scissor Enable (영역 클리핑용)
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = true;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		m_rasterizerStates[static_cast<size_t>(eRasterizerState::Scissor_CullNone)] =
		     m_graphics->CreateRasterizerState(desc);
	}

	void Renderer2D::createBlendStates()
	{
		// 1. Alpha Blend (RenderTarget[0] 기준, MRT 확장 대비 RenderTarget[1] 비블렌딩)
		D3D11_BLEND_DESC alphaDesc{};
		alphaDesc.AlphaToCoverageEnable = false;
		alphaDesc.IndependentBlendEnable = true;

		D3D11_RENDER_TARGET_BLEND_DESC colorBlendDesc{};
		colorBlendDesc.BlendEnable = true;
		colorBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		colorBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		colorBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		colorBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		colorBlendDesc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		colorBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		colorBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		alphaDesc.RenderTarget[0] = colorBlendDesc;

		D3D11_RENDER_TARGET_BLEND_DESC idBlendDesc{};
		idBlendDesc.BlendEnable = false;
		idBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		for (int i = 1; i < 8; ++i)
		{
			alphaDesc.RenderTarget[i] = idBlendDesc;
		}

		 m_blendStates[static_cast<size_t>(eBlendState::Alpha)] = m_graphics->CreateBlendState(alphaDesc);

		// 2. Additive Blend
		D3D11_BLEND_DESC additiveDesc{};
		additiveDesc.AlphaToCoverageEnable = false;
		additiveDesc.IndependentBlendEnable = true;

		D3D11_RENDER_TARGET_BLEND_DESC addColorDesc{};
		addColorDesc.BlendEnable = true;
		addColorDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		addColorDesc.DestBlend = D3D11_BLEND_ONE;
		addColorDesc.BlendOp = D3D11_BLEND_OP_ADD;
		addColorDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		addColorDesc.DestBlendAlpha = D3D11_BLEND_ONE;
		addColorDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		addColorDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		additiveDesc.RenderTarget[0] = addColorDesc;

		for (int i = 1; i < 8; ++i)
		{
			additiveDesc.RenderTarget[i] = idBlendDesc;
		}

		 m_blendStates[static_cast<size_t>(eBlendState::Additive)] = m_graphics->CreateBlendState(additiveDesc);
	}

	void Renderer2D::createSamplers()
	{
		D3D11_SAMPLER_DESC desc{};

		// Wrap + Point
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		m_samplerStates[static_cast<size_t>(eSamplerState::WrapPoint)] = m_graphics->CreateSamplerState(desc);

		// Wrap + Linear
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		m_samplerStates[static_cast<size_t>(eSamplerState::WrapLinear)] = m_graphics->CreateSamplerState(desc);

		// Clamp + Point (픽셀/도트 스프라이트·아틀라스용)
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		m_samplerStates[static_cast<size_t>(eSamplerState::ClampPoint)] = m_graphics->CreateSamplerState(desc);

		// Clamp + Linear (부드러운 2D 스프라이트·오프스크린/UI용)
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		m_samplerStates[static_cast<size_t>(eSamplerState::ClampLinear)] = m_graphics->CreateSamplerState(desc);
	}

	void Renderer2D::createResolutionDependantResources(uint32_t width, uint32_t height)
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		
		m_screenRenderTarget = std::make_unique<RenderTexture>(m_graphics, desc);
	}

	void Renderer2D::startBatch()
	{
		m_lineVertexCount = 0;	
		
		m_quadVertexCount = 0;
		m_quadIndexCount = 0;
	}

	void Renderer2D::flush()
	{
		m_graphics->SetViewport(0, 0, m_screenRenderTarget->GetWidth(), m_screenRenderTarget->GetHeight());

		if (m_lineVertexCount)
		{
			m_lineVertexBuffer->SetData(
				m_graphics,
				m_lineVertices.get(),
				m_lineVertexCount
			);

			auto sp = ShaderManager::Get().GetShaderProgram("DebugPrimitive");
			sp->Bind(m_graphics);

			m_cbFrame->Bind(m_graphics, eShaderStage::VS, 0);

			m_graphics->SetVertexBuffer(m_lineVertexBuffer.get());
			m_graphics->SetTopology(ePrimitiveTopology::LineList);

			m_graphics->Draw(m_lineVertexCount, 0);
		}

		if (m_quadIndexCount > 0)
		{
			m_quadVertexBuffer->SetData(
				m_graphics,
				m_quadVertices.get(),
				m_quadVertexCount
			);

			auto sp = ShaderManager::Get().GetShaderProgram("Sprite2D");
			sp->Bind(m_graphics);

			m_cbFrame->Bind(m_graphics, eShaderStage::VS, 0);
			//m_cbFrame->Bind(m_graphics, eShaderStage::PS, 0);

			// 0번(흰색)부터 m_textureSlotIndex - 1번까지 사용된 SRV 배열 바인딩
			// (m_graphics에 배열 바인딩 오버로드나 래퍼가 없다면 context->PSSetShaderResources(0, m_textureSlotIndex, srvs) 형태로 맞춰 사용)
			//m_graphics->SetShaderResourceViews(eShaderStage::PS, 0, m_textureSlotIndex, m_textureSlots);
			auto srv = TextureManager::Get().GetWhiteTextureView();
			m_graphics->SetShaderResourceView(eShaderStage::PS, 0, &srv);

			m_graphics->SetVertexBuffer(m_quadVertexBuffer.get());
			m_graphics->SetIndexBuffer(m_quadIndexBuffer.get());
			m_graphics->SetTopology(ePrimitiveTopology::TriangleList);

			m_graphics->DrawIndexed(m_quadIndexCount);
		}
	}

	void Renderer2D::nextBatch()
	{
		flush();
		startBatch();
	}
}