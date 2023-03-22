#include "divepch.h"
#include "Renderer.h"
#include "RendererEvents.h"
#include "Material.h"
#include "RenderPath.h"
#include "Technique.h"
#include "Viewport.h"
#include "View.h"
#include "Batch.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "Engine/EngineEvents.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/VertexBuffer.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	// 각종 Shader에 대응하는 ShaderVaritaion들의 배열을 만들어 놓는다.
	// 구분 기준은 vs, ps별 Input Defines인 듯 하다.


	Renderer::Renderer(Context* pContext)
		: Object(pContext),
		m_pGraphics(nullptr),
		m_pDefaultMaterial(nullptr),
		m_pDefaultRenderPath(nullptr),
		m_pDefaultTechnique(nullptr),
		m_bInitialized(false),
		m_pInstancingBuffer(nullptr)
	{
		// ScreenMode 메시지 구독

		// 스크린 모드가 설정되지 않았다면 스킵한단다..
		// 어떻게 스킵하는지 아직 확인 불가
		// 근데 내부에서 Graphics의 초기화를 확인한다.
		// 따라서 이 곳에서는 초기화를 수행할 수 없다.
		initialize();
	}

	Renderer::~Renderer()
	{
		DV_DELETE(m_pInstancingBuffer);

		// 그러고 보니 이것두 직접 제거하네...
		DV_DELETE(m_pDefaultRenderPath);

		// 왜인지 모르겠지만 이건 리소스에 등록을 안했다.
		DV_DELETE(m_pDefaultMaterial);

		for (int32_t i = 0; i < static_cast<int32_t>(m_Viewports.size()); ++i)
			DV_DELETE(m_Viewports[i]);
		m_Viewports.clear();

		DV_LOG_ENGINE_TRACE("Renderer 소멸 완료");
	}
	
	// EventHandler가 호출한다.
	void Renderer::Update(float delta)
	{
		m_Views.clear();

		for (auto i = static_cast<int32_t>(m_Viewports.size()) - 1; i >= 0; --i)
			queueViewport(nullptr, m_Viewports[i]);
		
		uint32_t numMainViewports = static_cast<uint32_t>(m_QueuedViewports.size());
		for (uint32_t i = 0; i < numMainViewports; ++i)
			updateQueuedViewport(i);

		FIRE_EVENT(RenderTargetUpdate());

		for (uint32_t i = numMainViewports; i < static_cast<uint32_t>(m_QueuedViewports.size()); ++i)
			updateQueuedViewport(i);

		m_QueuedViewports.clear();
	}
	
	void Renderer::Render()
	{
		// 임시: 현재 initialize() 호출할 곳이 없다.
		// 추후 이를 삭제하고 OnScreenMode로 처리.
		if (!m_bInitialized)
		{
			initialize();
		}

		// 역순으로 Views' Render.
		if (!m_Views.empty())
		{
			for (int32_t i = static_cast<int32_t>(m_Views.size() - 1); i >= 0; --i)
			{
				if (!m_Views[i])
					continue;

				m_Views[i]->Render();
			}
		}
	}

	void Renderer::OnRenderUpdate(const Event& e)
	{
		auto& evnt = dynamic_cast<const RenderUpdateEvent&>(e);

		Update(evnt.GetTimeStep());
	}

	// 이게 호출되려면 OnScreenModeChanged나 OnWindowResized가 호출되어야 한다.
	// orho는 SetScreenMode의 마지막에 OnScreenModeChagned를 호출한다.
	// OnScreenModeChanged는 설정을 로그로 출력하고, 데이터를 인자로 이벤트에 전달한다.
	// private인 것으로 보아 이때(SetScreenMode)만 호출되는 듯 하다.
	void Renderer::OnScreenMode(const Event& e)
	{
		if (!m_bInitialized)
			initialize();

		// 이미 생성되었다면 m_bViewReset = true;
	}
	
	RenderPath* Renderer::GetDefaultRenderPath() const
	{
		return m_pDefaultRenderPath;
	}

	void Renderer::SetDefaultRenderPath(RenderPath* pRenderPath)
	{
		if (pRenderPath)
			m_pDefaultRenderPath = pRenderPath;
	}

	// 원래는 const이다. 현재 직접 생성을 위해 제거했다.
	Technique* Renderer::GetDefaultTechnique()
	{
		// 비었다면 Cache에 파일명을 전달하여 직접생성한 후 설정
		// NoTexture.xml
		
		// 현재 최초 생성은 Material의 ResetToDefault()에서 구성된다.
		if (!m_pDefaultTechnique)
			m_pDefaultTechnique = GetSubsystem<ResourceCache>()->GetResource<Technique>("DefaultTech");

		return m_pDefaultTechnique;
	}

	void Renderer::SetDefaultTechnique(Technique* pTechnique)
	{
		if (pTechnique)
			m_pDefaultTechnique = pTechnique;
	}

	Viewport* Renderer::GetViewport(uint32_t index)
	{
		return m_Viewports.size() > index ? m_Viewports[index] : nullptr;
	}

	void Renderer::SetViewport(uint32_t index, Viewport* pViewport)
	{
		if (m_Viewports.size() >= index)
			m_Viewports.insert(m_Viewports.begin() + index, pViewport);
		else
			m_Viewports.emplace_back(pViewport);
	}

	// 이외에도 몇 가지를 더 전달받는다.
	Texture* Renderer::GetScreenBuffer(int32_t width, int32_t height, DXGI_FORMAT format, bool bCubemap)
	{
		// format으로 depthstencil 여부를 판별

		auto searchKey = static_cast<uint64_t>(format) << 32u | width << 24u | height << 16u;
		
		if (m_ScreenBuffers.find(searchKey) == m_ScreenBuffers.end())
			m_ScreenBufferIndices[searchKey] = 0;

		auto index = m_ScreenBufferIndices[searchKey];

		if (index >= static_cast<uint32_t>(m_ScreenBuffers[searchKey].size()))
		{
			Texture* pNewBuffer = nullptr;

			if (!bCubemap)
			{
				auto pNewTex2D = new Texture2D(m_pContext);
				// depthstencil 여부로 usage 설정.
				pNewTex2D->SetSize(width, height, format);

				pNewBuffer = dynamic_cast<Texture*>(pNewTex2D);
			}
			else
			{
				// cubemap 생성
			}

			// 기타 설정
			m_ScreenBuffers[searchKey].emplace_back(pNewBuffer);

			return pNewBuffer;
		}
		else
		{
			auto* pBuffer = m_ScreenBuffers[searchKey][index];
			// reset user time?
			return pBuffer;
		}

		return nullptr;
	}

	void Renderer::QueueViewportByRenderTarget(Texture* pRenderTarget)
	{
		if (!pRenderTarget)
			return;

		auto numViewports = pRenderTarget->GetViewportCount();
		for (uint32_t i = 0; i < numViewports; ++i)
			queueViewport(pRenderTarget, pRenderTarget->GetViewport(i));
	}

	void Renderer::SetBatchShaders(StaticBatch& batch, Technique& tech, bool bAllowShadow, const BatchQueue& queue)
	{
		// Tech의 Pass부분에 pass name, file path, defines가 저장되어 있다.
		Pass* pPass = batch.GetPass();

		// 이게 왜 단일 포인터가 아닌 벡터일까?
		auto vertexShaderVariations = pPass->GetVertexShaderVariations();
		auto pixelShaderVariations = pPass->GetPixelShaderVariations();

		if (vertexShaderVariations.empty() || pixelShaderVariations.empty())
		{
			// 여기서부터 LoadPassShaders다.
			// shader defines를 참조해 variation들을 만들거나 가져온다.

			vertexShaderVariations.clear();
			pixelShaderVariations.clear();

			// shader defines부터 구성한다.

			vertexShaderVariations.emplace_back(
				m_pGraphics->GetShader(eShaderType::Vertex, pPass->GetVertexShaderName(), pPass->GetVertexShaderDefines()));

			pixelShaderVariations.emplace_back(
				m_pGraphics->GetShader(eShaderType::Pixel, pPass->GetPixelShaderName(), pPass->GetPixelShaderDefines()));
		}

		if (!vertexShaderVariations.empty() && !pixelShaderVariations.empty())
		{
			// batch geometry를 다시 설정한다. 이게 최종일 듯.

			// 이후 현재 batch가 사용하는 최종 variation을 설정한다.
			batch.SetVertexShaderVariation(vertexShaderVariations[0]);
			batch.SetPixelShaderVariation(pixelShaderVariations[0]);
		}

		if (!batch.GetVertexShaderVariation() || !batch.GetPiexelShaderVariation())
		{
			DV_LOG_ENGINE_ERROR("Renderer::SetBatchShaders - ");
		}
	}

	void Renderer::initialize()
	{
		auto pGraphics = GetSubsystem<Graphics>();
		auto pCache = GetSubsystem<ResourceCache>();
		if (!pGraphics || !pGraphics->IsInitialized() || !pCache)
			return;

		m_pGraphics = pGraphics;

		// cache manager로 부터 load
		// default light ramp
		// default light spot

		// default material 생성: 내부에서 직접 초기화한다.
		m_pDefaultMaterial = new Material(m_pContext);

		// 객체를 생성한 후 cache manager로부터 load
		// 그런데 왜 캐스로 부터 가져온 후 로드를 하는거지?
		// default render path
		m_pDefaultRenderPath = new RenderPath(m_pContext);
		{
			// 실제로는 forward.xml을 로드
			{
				RenderPathCommand command;
				command.Type = eRenderCommandType::Clear;
				command.ClearColor = { 0.05f, 0.1f, 0.15f, 1.0f };// { 0.1f, 0.1f, 0.1f, 1.0f };
				command.ClearDepth = 1.0f;
				command.ClearStencil = 0;
				command.Outputs.resize(1);
				command.Outputs.emplace_back("viewport", PositiveX);	// 원래는 Load에서 초기화된다.
				m_pDefaultRenderPath->AddCommand(command);
			}

			{
				RenderPathCommand command;
				command.Type = eRenderCommandType::ScenePass;
				command.Pass = "base";
				//m_pDefaultRenderPath->AddCommand(command);
			}

			{
				RenderPathCommand command;
				command.Type = eRenderCommandType::ScenePass;
				// 얘네를 gpu로 보내야 하는데 어디서 수행할까?
				command.Pass = "alpha";
				command.bVertexLights = "true";
				// sort와 meta data도 가진다.
				m_pDefaultRenderPath->AddCommand(command);
			}
		}

		// create geometries
		// create instancing buffer
		createInstancingBuffer();

		// reset shadow map
		// reset buffers

		m_bInitialized = true;

		SUBSCRIBE_EVENT(eEventType::RenderUpdate, EVENT_HANDLER_PARAM(OnRenderUpdate));

		DV_LOG_ENGINE_TRACE("Renderer 초기화 성공");
	}

	// 하나의 오브젝트를 여러번 그리는 것이니
	// 개별 버퍼가 아니라 하나의 버퍼로 관리하는 것이 효율적이다.
	// 그렇다면 대상 오브젝트의 Elements 구성을 유지한 체
	// 지오메트리 정보 또한 복사를 하고
	// 다시 매 오브젝트마다 위치 변환 행렬을 lock/unlock해야 하는데...
	// => 지오메트리 정보를 넣지 않는다.
	// 변환행렬과 추가 인스턴스 데이터만 관리하는 버퍼이다.
	// 그리고 SetVertexBuffers를 사용하여 배열로 적용시킨다.
	void Renderer::createInstancingBuffer()
	{
		m_pInstancingBuffer = new VertexBuffer(m_pContext);
		// elements를 구성한 후
		// 최대개수 = 1024로 설정한다.
		// ResizeInstancingBuffer()로 전달받은 개수 이상의 2진수 배열 크기로 만드는 함수도 필요하다.
	}

	void Renderer::queueViewport(Texture* pRenderTarget, Viewport* pViewport)
	{
		if (!pViewport)
			return;

		m_QueuedViewports.emplace_back(std::make_pair(pRenderTarget, pViewport));
	}

	void Renderer::updateQueuedViewport(uint32_t index)
	{
		Texture* pRenderTarget = m_QueuedViewports[index].first;
		Viewport* pViewport = m_QueuedViewports[index].second;

		if (!pViewport->GetView())
			pViewport->AllocateView();

		auto* pView = pViewport->GetView();
		DV_ASSERT(pView);

		if (!pView->Define(dynamic_cast<RenderTexture*>(pRenderTarget), pViewport))
			return;

		m_Views.emplace_back(pView);

		// 원래는 FrameInfo를 전달한다.
		// 그리고 이는 멤버 변수이므로, 여기에서 호출이 가능하다.
		pView->Update(1.0f);
	}
}