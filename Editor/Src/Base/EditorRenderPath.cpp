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
		auto pRenderTargetView = Dive::Renderer::GetSampleTexture()->GetRenderTargetView();
		auto pDepthStencilView = Dive::Renderer::GetDepthStencilTexture()->GetDepthStencilView();
		if (!pImmediateContext || !pRenderTargetView)
			return;

		float clearColors[4] = { 0.35f, 0.35f, 0.7f, 1.0f };

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		if (pDepthStencilView)
			pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		UINT numViews = 8;
		std::array<ID3D11RenderTargetView*, 8> renderTargetViews = { nullptr };
		renderTargetViews[0] = pRenderTargetView;
		//pImmediateContext->OMSetRenderTargets(1, renderTargetViews.data(), pDepthStencilView);
		//pImmediateContext->RSSetViewports(1, Dive::Renderer::GetSampleTexture()->GetViewport());
	}

	Dive::CommandList cl;

	// ī�޶��� ������ �̰� ���� ũ��.
	// �̿ܿ��� Ŭ���� ����� �ֱ� �ѵ�... �װ� viewport�ϱ�? �ƴϸ� frustum�ϱ�?
	// ��·�簣�� pass�ȿ� �ִ°� ���ƺ��δ�.
	// �׷��ٸ� �ᱹ SceneViewCamera�� Camera�� ����ؾ� �Ѵ�.
	auto view = m_pSceneViewCamera->GetViewMatrix();
	auto proj = m_pSceneViewCamera->GetProjectionMatrix();

	auto pImmediateContext = Dive::Renderer::GetGraphicsDevice().GetImmediateContext();

	// �� ������ �� ���� bind�ϸ� ��� Shader���� ����� �� �ִ�.
	// FrameBuffer
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		auto pFrameBuffer = Dive::Renderer::GetMatrixBuffer();

		// map
		pImmediateContext->Map(pFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		Dive::FrameBuffer* pPtr = static_cast<Dive::FrameBuffer*>(mappedResource.pData);

		pPtr->view = DirectX::XMMatrixTranspose(view);
		pPtr->proj = DirectX::XMMatrixTranspose(proj);

		// unmap
		pImmediateContext->Unmap(pFrameBuffer, 0);

		pImmediateContext->VSSetConstantBuffers(0, 1, &pFrameBuffer);
	}

	Dive::RenderPath::passDefault(&cl);

	// BeginScene�� ��ǻ� �����ϱ� �ָ��ϴ�,
	// EndScene���� Present��� �̸��� ��︰��.
	// �׷��� �� ������ ȣ���ϸ� �ȵȴ�.
	// ImGui�� Present�� �� ������ �߻��Ѵ�.
	//Dive::Renderer().EndScene();
}
