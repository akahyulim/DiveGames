#include "EditorRenderPath.h"
#include "SceneViewCamera.h"

void EditorRenderPath::Update(float delta)
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	// �� �� ��ġ�� �ָ��ϴ�. 
	// �׷��ٰ� Renderer�� Update�� �δ� �͵� �̻��ѵ�...
	m_pSceneViewCamera->Update(delta);
	m_pActiveScene->Update(delta);

	m_MainVisibilities;
	m_MainVisibilities.pScene = m_pActiveScene;
	Dive::Renderer::UpdateVisibility(m_MainVisibilities);
}

void EditorRenderPath::Render()
{
	if (!m_pActiveScene || !m_pSceneViewCamera)
		return;

	// begin scene
	// : ���� ���� bind�ϰ� �ִ�. 
	{
		// �̰͵� Pass �ȿ� �־�� �ұ�?
		// ���ĸ�ź�� ��� �ʱ�ȭ�� �ܺο��� �ϰ�
		// pass�� �ʿ��� �͵��� ���ڷ� ������ �� bind�ϴ� �� ����.
		auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();
		auto pRenderTargetView = Dive::Renderer::GetGBufferAlbedo()->GetRenderTargetView();
		auto pDepthStencilView = Dive::Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		if (!pImmediateContext || !pRenderTargetView)
			return;

		float clearColors[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		UINT numViews = 8;
		std::array<ID3D11RenderTargetView*, 8> renderTargetViews = { nullptr };
		renderTargetViews[0] = pRenderTargetView;
		//pImmediateContext->OMSetRenderTargets(1, renderTargetViews.data(), pDepthStencilView);
		//pImmediateContext->RSSetViewports(1, Dive::Renderer::GetGBufferAlbedo()->GetViewport());
	}

	Dive::CommandList cl;

	// ī�޶��� ������ �̰� ���� ũ��.
	// �̿ܿ��� Ŭ���� ����� �ֱ� �ѵ�... �װ� viewport�ϱ�? �ƴϸ� frustum�ϱ�?
	// ��·�簣�� pass�ȿ� �ִ°� ���ƺ��δ�.
	// �׷��ٸ� �ᱹ SceneViewCamera�� Camera�� ����ؾ� �Ѵ�.
	auto view = m_pSceneViewCamera->GetViewMatrix();
	auto proj = m_pSceneViewCamera->GetProjectionMatrix();
	auto eyePos = m_pSceneViewCamera->GetPosition();

	auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();

	// �� ������ �� ���� bind�ϸ� ��� Shader���� ����� �� �ִ�.
	// FrameBuffer
	{
		auto pCbFrame = Dive::Renderer::GetCbFrame();

		// map & unmap
		auto pPtr = static_cast<Dive::FrameBuffer*>(pCbFrame->Map());
		pPtr->view = DirectX::XMMatrixTranspose(view);
		pPtr->proj = DirectX::XMMatrixTranspose(proj);
		
		{
			// linear depth ��꿡 ���ȴ�.
			// ��Ȯ�� ���� ������ ���� �𸥴�.
			DirectX::XMFLOAT4X4 p;
			DirectX::XMStoreFloat4x4(&p, proj);
			pPtr->perspectiveValues.x = 1.0f / p._11;
			pPtr->perspectiveValues.y = 1.0f / p._22;
			pPtr->perspectiveValues.z = p._43;
			pPtr->perspectiveValues.w = -p._33;
		}

		pPtr->eyePos = eyePos;
		pCbFrame->Unmap();

		cl.SetConstantBuffer(Dive::Scope_Vertex | Dive::Scope_Pixel, Dive::eConstantBufferSlot::Frame, pCbFrame);
	}

	// �ϴ� �ϵ� �ڵ�
	auto pPointSampler = Dive::Renderer::GetSamplerState(Dive::eSamplerStateType::Point);
	auto pLinearSampler = Dive::Renderer::GetSamplerState(Dive::eSamplerStateType::Linear);
	pImmediateContext->PSSetSamplers(0, 1, &pPointSampler);
	pImmediateContext->PSSetSamplers(1, 1, &pLinearSampler);

	//Dive::RenderPath::passDefault(&cl);
	Dive::RenderPath::passGBuffer(&cl);
	Dive::RenderPath::passLighting(&cl);

	// BeginScene�� ��ǻ� �����ϱ� �ָ��ϴ�,
	// EndScene���� Present��� �̸��� ��︰��.
	// �׷��� �� ������ ȣ���ϸ� �ȵȴ�.
	// ImGui�� Present�� �� ������ �߻��Ѵ�.
	//Dive::Renderer().EndScene();
}
