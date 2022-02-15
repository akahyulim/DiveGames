#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
	m_pRenderer = &Dive::GetRenderer();

	//m_pSample = Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");

	// ShaderResourceView�� ������ Device�� �ٸ��� �ȵǳ�����.
	// ��, GraphicsDevice�� ������ �ȵȴ�...
	{
		std::string path = "Assets/Textures/DokeV.jpeg";

		// ���� Ȯ��
		if (!std::filesystem::exists(path))
		{
			DV_CORE_WARN("{:s}�� �������� �ʴ� �����Դϴ�.", path);
		}

		std::wstring filePath(path.begin(), path.end());
		DirectX::ScratchImage image;

		HRESULT hResult;

		// ����... �̰� �������ٴ�...
		hResult = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

		hResult = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
		if (FAILED(hResult))
		{
			DV_CORE_WARN("fail to load image file: {:s}", path);
		}
		
		hResult = DirectX::CreateShaderResourceView(m_pEditor->GetD3dDevice(), image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			&m_pSample);
		if (FAILED(hResult))
		{
			DV_CORE_WARN("ShaderResourceView ������ �����Ͽ����ϴ�.");
			return;
		}
	}
}

// Scene�� �ִٸ�
// Enigne�� Backbuffer���� rtv�� ���� �׸���.
void ScenePanel::renderWindow()
{
	if (!m_pRenderer)
		return;

	// ũ�⸦ ����Ѵ�. => �� �𸣰ڴ�.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer�� viewport�� �����Ѵ�.
	// Editor�� Camera�� �Բ� ����??

	// ũ�Ⱑ ����Ǿ��ٸ� renderer�� �ػ󵵸� �����Ѵ�.
	m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	// image�� ������ �׸���.
	ImGui::Image(
		m_pSample,	// shader resource view��...
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
