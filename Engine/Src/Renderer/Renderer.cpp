#include "DivePch.h"
#include "Renderer.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderVariation.h"
#include "Graphics/InputLayout.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/RenderTexture.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

namespace Dive
{
	static DirectX::XMFLOAT2 s_ResolutionRender;

	static std::unordered_map<eRendererGameObject, std::vector<GameObject*>> s_Renderables;
	static Camera* s_pCamera = nullptr;

	static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> s_RasterizerStates;
	static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> s_DepthStencilStates;
	static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> s_BlendStates;

	static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> s_RenderTargets;
	// shader
	// sampler?
	// constant buffer

	// 현재 vertex와 pixel이 나뉘어 있어 조금 복잡하다.
	// 그리고 추후 inputlayout별로 생성되면 더 복잡해질 수 있다.
	static ShaderVariation* s_pBasicVertexShader = nullptr;
	static ShaderVariation* s_pBasicPixelShader = nullptr;
	static ShaderVariation* s_pBasicSkinnedVertexShader = nullptr;
	static ShaderVariation* s_pBasicSkinnedPixelShader = nullptr;
	static ShaderVariation* s_pForwardLightVertexShader = nullptr;
	static ShaderVariation* s_pDirectionalLightPixelShader = nullptr;
	static ShaderVariation* s_pPointLightPixelShader = nullptr;
	static ShaderVariation* s_pDeferredShadingVertexShader = nullptr;
	static ShaderVariation* s_pDeferredShadingPixelShader = nullptr;
	static ShaderVariation* s_pDeferredSkinnedShadingVertexShader = nullptr;
	static ShaderVariation* s_pDeferredSkinnedShadingPixelShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightVertexShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightPixelShader = nullptr;

	static ConstantBuffer* s_pCameraVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pModelVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pCameraPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pLightPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pMaterialPixelShaderBuffer = nullptr;

	bool Renderer::Initialize()
	{
		// 현재 이 부분때문에 createRenderTargets가 두 번 호출된다.
		// 이건 스파르탄도 거의 동일하다.
		SetResolutionRender(Graphics::GetWindowSize());

		if (!createShaders())
		{
			DV_CORE_WARN("셰이더 객체 생성에 실패하였습니다.");
		}

		if (!createConstantBuffers())
		{
			DV_CORE_WARN("CreateBuffers 생성에 실패하였습니다.");
		}

		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
		
		createRenderTargets();
		
		DV_CORE_INFO("Renderer 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		{
			DV_DELETE(s_pMaterialPixelShaderBuffer);
			DV_DELETE(s_pLightPixelShaderBuffer);
			DV_DELETE(s_pCameraPixelShaderBuffer);
			DV_DELETE(s_pModelVertexShaderBuffer);
			DV_DELETE(s_pCameraVertexShaderBuffer);

			DV_DELETE(s_pDirectionalLightPixelShader);
			DV_DELETE(s_pPointLightPixelShader);
			DV_DELETE(s_pForwardLightVertexShader);
		}

		for (auto* pRasterizerState : s_RasterizerStates)
			DV_RELEASE(pRasterizerState);

		for (auto* pDepthStencilState : s_DepthStencilStates)
			DV_RELEASE(pDepthStencilState);

		for (auto* pBlendState : s_BlendStates)
			DV_RELEASE(pBlendState);

		for (auto* pRenderTarget : s_RenderTargets)
			DV_DELETE(pRenderTarget);
		

		DV_CORE_INFO("Renderer 종료에 성공하였습니다.");
	}

	void Renderer::Update()
	{
		// 매 프레임 실행된다.
		// 따라서 렌더링을 마친 후 초기화 되어야 한다.
		s_Renderables.clear();
		s_pCamera = nullptr;

		auto allGameObjects = Scene::GetAllGameObjects();
		for (auto* pGameObject : allGameObjects)
		{
			if (Camera* pCamera = pGameObject->GetComponent<Camera>())
			{
				s_Renderables[eRendererGameObject::Camera].emplace_back(pGameObject);
				s_pCamera = pCamera;
			}

			if (Light* pLight = pGameObject->GetComponent<Light>())
			{
				s_Renderables[eRendererGameObject::Light].emplace_back(pGameObject);
			}

			if (MeshRenderer* pMeshRenderer = pGameObject->GetComponent<MeshRenderer>())
			{
				s_Renderables[eRendererGameObject::Geometry].emplace_back(pGameObject);
			}

			if (SkinnedMeshRenderer* pSkinnedMeshRenderer = pGameObject->GetComponent<SkinnedMeshRenderer>())
			{
				s_Renderables[eRendererGameObject::SkinnedGeometry].emplace_back(pGameObject);
			}
		}
		// Gemetries를 Camera 기준으로 정렬한다. 투명, 불투명을 따로 한다.
		// 나같은 경우 Skinned도 따로 해야 한다.

		// 각종 pass 구분
		// 일단 render target이 필요하다. backbuffer로는 안된다.
		// 카메라가 render target을 가질 수도 있다.
		
		// Deferred 1 - GBuffer에 MeshRenderer를 그린다.
		{
			// pre render
			ID3D11RenderTargetView* pRenderTextures[] = {
				GetRenderTarget(eRenderTarget::GBuffer_Color_SpecIntensity)->GetRenderTargetView(),
				GetRenderTarget(eRenderTarget::GBuffer_Normal)->GetRenderTargetView(),
				GetRenderTarget(eRenderTarget::GBuffer_SpecPower)->GetRenderTargetView()
			};
			Graphics::SetRenderTargetViews(0, 3, pRenderTextures);
			Graphics::SetDepthStencilView(GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)->GetDepthStencilView());
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil, s_pCamera->GetBackgroundColor(), 1.0, 0);
			Graphics::SetDepthStencilState(s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)], 1);

			// camera 정보
			D3D11_VIEWPORT viewport = s_pCamera->GetViewport();
			Dive::Graphics::GetDeviceContext()->RSSetViewports(1, &viewport);
			auto pMappedData = static_cast<CameraVertexShaderBuffer*>(s_pCameraVertexShaderBuffer->Map());
			pMappedData->viewMatrix = DirectX::XMMatrixTranspose(s_pCamera->GetViewMatrix());
			pMappedData->projMatrix = DirectX::XMMatrixTranspose(s_pCamera->GetProjectionMatrix());
			s_pCameraVertexShaderBuffer->Unmap();
			Dive::Graphics::SetConstantBuffer(0, Dive::eShaderType::VertexShader, s_pCameraVertexShaderBuffer);

			// MeshRenderer 그리기
			for (auto* pMeshRenderer : s_Renderables[eRendererGameObject::Geometry])
			{
				pMeshRenderer->GetComponent<MeshRenderer>()->Draw();
			}
			for (auto* pSkinnedMeshRenderer : s_Renderables[eRendererGameObject::SkinnedGeometry])
			{
				pSkinnedMeshRenderer->GetComponent<SkinnedMeshRenderer>()->Draw();
			}

			// post render
			ID3D11RenderTargetView* pRenderTargetViews[] = { nullptr, nullptr, nullptr };
			Graphics::SetRenderTargetViews(0, 3, pRenderTargetViews);
			Graphics::SetDepthStencilView(GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)->GetDepthStencilViewReadOnly());
		}

		// Deferred 2 - GBuffer의 정보와 광원을 계산하여 타겟에 출력한다.
		// 이걸 디퍼드라고 하기보단 Lighting이라고 하는 편이 나을 것 같기도...
		{
			// pre render
			Graphics::SetRenderTargetView(0, GetRenderTarget(eRenderTarget::FrameRender)->GetRenderTargetView());
			Graphics::SetDepthStencilView(Graphics::GetDefaultDepthStencilView());	// 이건 좀 에바?
			Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);

			// camera
			DirectX::XMFLOAT3 pos;
			auto pBuffer = Renderer::GetCameraPixelShaderBuffer();
			auto pMappedData = static_cast<CameraPixelShaderBuffer*>(pBuffer->Map());
			DirectX::XMStoreFloat3(&pos, s_pCamera->GetGameObject()->GetTransform()->GetPosition());
			pMappedData->cameraPos = pos;
			DirectX::XMFLOAT4X4 proj;
			DirectX::XMStoreFloat4x4(&proj, s_pCamera->GetProjectionMatrix());
			pMappedData->perspectiveValues.x = 1.0f / proj._11;
			pMappedData->perspectiveValues.y = 1.0f / proj._22;
			pMappedData->perspectiveValues.z = proj._43;
			pMappedData->perspectiveValues.w = -proj._33;
			pMappedData->viewInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, s_pCamera->GetViewMatrix()));
			pBuffer->Unmap();
			Graphics::SetConstantBuffer(0, eShaderType::PixelShader, pBuffer);

			// 여기서부터 광원 유무에 따라 처리가 달라져야 한다.
			{
				// 기존에는 아에 이 부분을 건너 띄었지만
				// diff만 출력하는 것도 한 방법이다.

				// 광원이 존재한다면 루프를 돌려야 한다.
				// for (uint32_t i = 0; i < static_cast<uint32_t>(m_Lights.size()); ++i)
				{
					// 그리고 광원이 첫 번째이냐 아니냐에 따라
					// DepthStencil과 Blend State가 달라진다.
					//if (i == 0)
					//	Graphics::SetDepthStencilState(Renderer::GetForwardLightDS(), 0);
					//else if (i == 1)
					//	Graphics::GetDeviceContext()->OMSetBlendState(Renderer::GetBlendState(), NULL, 0xffffffff);

					// 광원에 대한 정보를 GPU로 전달한다. Constant Buffer로 말이다.
					// 그런데 현재 DirLight만 구현해 놓았다. 
					//auto pLight = m_Lights[i];
					//auto pBuffer = Renderer::GetLightPixelShaderBuffer();
					//auto pMappedData = static_cast<LightPixelShaderBuffer*>(pBuffer->Map());
					//DirectX::XMFLOAT3 pos;
					//DirectX::XMStoreFloat3(&pos, pLight->GetGameObject()->GetTransform()->GetPosition());
					//pMappedData->lightPos = pos;
					//pMappedData->lightRange = 1.0f / pLight->GetRange();
					//pMappedData->lightColor = pLight->GetColor();
					//pMappedData->lightDir = pLight->GetDir();
					//pBuffer->Unmap();
					//Graphics::SetConstantBuffer(1, eShaderType::PixelShader, pBuffer);

					Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, static_cast<Texture*>(GetRenderTarget(eRenderTarget::GBuffer_DepthStencil)));
					Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, static_cast<Texture*>(GetRenderTarget(eRenderTarget::GBuffer_Color_SpecIntensity)));
					Graphics::SetTexture(eTextureUnit::GBuffer_Normal, static_cast<Texture*>(GetRenderTarget(eRenderTarget::GBuffer_Normal)));
					Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, static_cast<Texture*>(GetRenderTarget(eRenderTarget::GBuffer_SpecPower)));

					// RenderTarget에 그리는 shader 전달
					Graphics::SetShaderVariation(eShaderType::VertexShader, GetDeferredDirLightVertexShaderVariation());
					Graphics::SetShaderVariation(eShaderType::PixelShader, GetDeferredDirLightPixelShaderVariation());

					Graphics::SetVertexBuffer(nullptr);
					Graphics::Draw(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, 0);

					// clean up
					// 이 부분이 꼭 필요하다. 이유는 아직 모르겠다.
					{
						Graphics::SetTexture(eTextureUnit::GBuffer_DepthStencil, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_Color_SpecIntensity, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_Normal, nullptr);
						Graphics::SetTexture(eTextureUnit::GBuffer_SpecPower, nullptr);

						Graphics::SetDepthStencilState(nullptr, 0);
						//Graphics::GetDeviceContext()->OMSetBlendState(NULL, nullptr, 0xffffffff);
					}
				}
			}
			//Graphics::SetShaderVariation(eShaderType::VertexShader, nullptr);
			//Graphics::SetShaderVariation(eShaderType::PixelShader, nullptr);
		}
	}

	DirectX::XMFLOAT2 Renderer::GetResolutionRender()
	{
		return s_ResolutionRender;
	}
	
	void Renderer::SetResolutionRender(uint32_t width, uint32_t height)
	{
		if (s_ResolutionRender.x == static_cast<float>(width) || s_ResolutionRender.y == static_cast<float>(height))
			return;

		s_ResolutionRender.x = static_cast<float>(width);
		s_ResolutionRender.y = static_cast<float>(height);

		createRenderTargets();
		// 스파르탄은 샘플러도 새로 만든다.
		// 내 경우 샘플러가 텍스처에 있던가...?
	}

	void Renderer::SetResolutionRender(DirectX::XMINT2 size)
	{
		SetResolutionRender(size.x, size.y);
	}

	RenderTexture* Renderer::GetRenderTarget(const eRenderTarget renderTarget)
	{
		return s_RenderTargets[static_cast<size_t>(renderTarget)];
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState(const eDepthStencilState state)
	{
		return s_DepthStencilStates[static_cast<size_t>(state)];
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState(const eRasterizerState state)
	{
		return s_RasterizerStates[static_cast<size_t>(state)];
	}

	ID3D11BlendState* Renderer::GetBlendState(const eBlendState state)
	{
		return s_BlendStates[static_cast<size_t>(state)];
	}

	ShaderVariation* Renderer::GetBasicVertexShaderVariation()
	{
		return s_pBasicVertexShader;
	}

	ShaderVariation* Renderer::GetBasicPixelShaderVariation()
	{
		return s_pBasicPixelShader;
	}

	ShaderVariation* Renderer::GetBasicSkinnedVertexShaderVariation()
	{
		return s_pBasicSkinnedVertexShader;
	}

	ShaderVariation* Renderer::GetBasicSkinnedPixelShaderVariation()
	{
		return s_pBasicSkinnedPixelShader;
	}

	ShaderVariation* Renderer::GetForwardLightVertexShaderVariation()
	{
		return s_pForwardLightVertexShader;
	}

	ShaderVariation* Renderer::GetDirectionalLightPixelShaderVariation()
	{
		return s_pDirectionalLightPixelShader;
	}

	ShaderVariation* Renderer::GetPointLightPixelShaderVariation()
	{
		return s_pPointLightPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredShadingVertexShaderVariation()
	{
		return s_pDeferredShadingVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredShadingPixelShaderVariation()
	{
		return s_pDeferredShadingPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredSkinnedShadingVertexShaderVariation()
	{
		return s_pDeferredSkinnedShadingVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredSkinnedShadingPixelShaderVariation()
	{
		return s_pDeferredSkinnedShadingPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightVertexShaderVariation()
	{
		return s_pDeferredDirLightVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightPixelShaderVariation()
	{
		return s_pDeferredDirLightPixelShader;
	}

	ConstantBuffer* Renderer::GetCameraVertexShaderBuffer()
	{
		return s_pCameraVertexShaderBuffer;
	}

	ConstantBuffer* Renderer::GetModelVertexShaderBuffer()
	{
		return s_pModelVertexShaderBuffer;
	}

	ConstantBuffer* Renderer::GetCameraPixelShaderBuffer()
	{
		return s_pCameraPixelShaderBuffer;
	}

	ConstantBuffer* Renderer::GetLightPixelShaderBuffer()
	{
		return s_pLightPixelShaderBuffer;
	}

	ConstantBuffer* Renderer::GetMaterialPixelShaderBuffer()
	{
		return s_pMaterialPixelShaderBuffer;
	}

	void Renderer::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;

		// FillSolid_CullBack
		ZeroMemory(&desc, sizeof(desc));
		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.ScissorEnable = FALSE;
		desc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullBack)])))
		{
			DV_CORE_ERROR("RasterizerState FillSolid_CullBack 생성에 실패하였습니다.");
		}
	}

	// DepthWriteMask에서 ZERO가 끄기, ALL이 켜기. READ는 기본?
	void Renderer::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc;

		// DepthReadWrite
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;
		
			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite 생성에 실패하였습니다.");
			}
		}

		// DepthReadWrite_StencilReadWrite
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite_StencilReadWrite 생성에 실패하였습니다.");
			}
		}

		// GBuffer에도 하나 있다.
		// DepthReadWrite_StencilReadWrite
		// 하지만 Face 설정이 위와 다르다.
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
			{
				DV_CORE_ERROR("DepthStencilState GBuffer 생성에 실패하였습니다.");
			}
		}
	}

	void Renderer::createBlendStates()
	{
		// Addictive
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			TRUE,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			desc.RenderTarget[i] = defaultRenderTargetBlendDesc;

		if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &s_BlendStates[static_cast<size_t>(eBlendState::Addictive)])))
		{
			DV_CORE_ERROR("BlandState Addictive 생성에 실패하였습니다.");
		}
	}

	// 유니티의 경우 material에서 선택하는 shader는 legacy와 standard 두 가지로만 나뉜다.
	// Light 및 post procssing은 어떻게 적용하는지 아직 모르겠다.
	bool Renderer::createShaders()
	{
		// basic
		{
			s_pBasicVertexShader = new ShaderVariation;
			if (!s_pBasicVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/Basic.hlsl", eVertexType::Model))
				return false;
			s_pBasicPixelShader = new ShaderVariation;
			if (!s_pBasicPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/Basic.hlsl"))
				return false;

			s_pBasicSkinnedVertexShader = new ShaderVariation;
			if (!s_pBasicSkinnedVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/Basic_Skinned.hlsl", eVertexType::Skinned))
				return false;
			s_pBasicSkinnedPixelShader = new ShaderVariation;
			if (!s_pBasicSkinnedPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/Basic_Skinned.hlsl"))
				return false;
		}

		// forward light
		{
			s_pForwardLightVertexShader = new ShaderVariation;
			if (!s_pForwardLightVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/ForwardLightCommon.hlsl", eVertexType::Model))
				return false;
			s_pDirectionalLightPixelShader = new ShaderVariation;
			if (!s_pDirectionalLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/DirectionalLight.hlsl"))
				return false;
			s_pPointLightPixelShader = new ShaderVariation;
			if (!s_pPointLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/PointLight.hlsl"))
				return false;
		}

		// Deferred Shading
		{
			s_pDeferredShadingVertexShader = new ShaderVariation;
			if (!s_pDeferredShadingVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/Deferred.hlsl", eVertexType::Model))
				return false;
			s_pDeferredShadingPixelShader = new ShaderVariation;
			if (!s_pDeferredShadingPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/Deferred.hlsl"))
				return false;

			s_pDeferredSkinnedShadingVertexShader = new ShaderVariation;
			if (!s_pDeferredSkinnedShadingVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/DeferredSkinned.hlsl", eVertexType::Skinned))
				return false;
			s_pDeferredSkinnedShadingPixelShader = new ShaderVariation;
			if (!s_pDeferredSkinnedShadingPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/DeferredSkinned.hlsl"))
				return false;

			s_pDeferredDirLightVertexShader = new ShaderVariation;
			if (!s_pDeferredDirLightVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/DeferredDirLight.hlsl"))
				return false;
			s_pDeferredDirLightPixelShader = new ShaderVariation;
			if (!s_pDeferredDirLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/DeferredDirLight.hlsl"))
				return false;
		}

		return true;
	}

	bool Renderer::createConstantBuffers()
	{
		// camera vertex shader
		{
			s_pCameraVertexShaderBuffer = new ConstantBuffer("CameraVertexShaderBuffer");
			if (!s_pCameraVertexShaderBuffer->Create<CameraVertexShaderBuffer>())
				return false;
		}

		// model vertex shader
		{
			s_pModelVertexShaderBuffer = new ConstantBuffer("ModelVertexShaderBuffer");
			if (!s_pModelVertexShaderBuffer->Create<ModelVertexShaderBuffer>())
				return false;
		}

		// camera pixel shader
		{
			s_pCameraPixelShaderBuffer = new ConstantBuffer("CameraPixelShaderBuffer");
			if (!s_pCameraPixelShaderBuffer->Create<CameraPixelShaderBuffer>())
				return false;
		}

		// light pixel shader
		{
			s_pLightPixelShaderBuffer = new ConstantBuffer("LightPixelShaderBuffer");
			if (!s_pLightPixelShaderBuffer->Create<LightPixelShaderBuffer>())
				return false;
		}

		// material pixel shader
		{
			s_pMaterialPixelShaderBuffer = new ConstantBuffer("MaterialPixelShaderBuffer");
			if (!s_pMaterialPixelShaderBuffer->Create<MaterialPixelShaderBuffer>())
				return false;
		}

		return true;
	}

	void Renderer::createRenderTargets()
	{
		for (auto* pRenderTarget : s_RenderTargets)
			DV_DELETE(pRenderTarget);

		int width = static_cast<int>(s_ResolutionRender.x);
		int height = static_cast<int>(s_ResolutionRender.y);

		// GBuffer_DepthStencil
		auto* pGbufferDepthStencil = new RenderTexture;
		pGbufferDepthStencil->SetDepthStencil(width, height, DXGI_FORMAT_R24G8_TYPELESS, true);
		pGbufferDepthStencil->SetFilter(D3D11_FILTER_MIN_MAG_MIP_POINT);
		pGbufferDepthStencil->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_DepthStencil)] = pGbufferDepthStencil;
			
		// GBuffer_Color_SpecIntensity
		auto* pGBufferColorSpecIntensity = new RenderTexture;
		pGBufferColorSpecIntensity->SetRenderTarget(width, height);
		pGBufferColorSpecIntensity->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_Color_SpecIntensity)] = pGBufferColorSpecIntensity;
			
		// GBuffer_Normal
		auto* pGBufferNormal = new RenderTexture;
		pGBufferNormal->SetRenderTarget(width, height, DXGI_FORMAT_R11G11B10_FLOAT);
		pGBufferNormal->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_Normal)] = pGBufferNormal;

		// GBuffer_SpecPower
		auto* pGBufferSpecPower = new RenderTexture;
		pGBufferSpecPower->SetRenderTarget(width, height);
		pGBufferSpecPower->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_SpecPower)] = pGBufferSpecPower;

		// FrameRender
		auto* pFrameRender = new RenderTexture;
		pFrameRender->SetRenderTarget(width, height);
		s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameRender)] = pFrameRender;
			
		// FrameOutput

		DV_CORE_INFO("RenderTargets({0:d} x {1:d})를 생성 또는 갱신하였습니다.", width, height);
	}
}