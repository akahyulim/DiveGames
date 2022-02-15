#include "ScenePanel.h"

ScenePanel::ScenePanel(Editor* pEditor)
	: Panel(pEditor, "Scene")
{
	m_pRenderer = &Dive::GetRenderer();

	//m_pSample = Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");

	// ShaderResourceView를 생성한 Device가 다르면 안되나보다.
	// 즉, GraphicsDevice를 나누면 안된다...
	{
		std::string path = "Assets/Textures/DokeV.jpeg";

		// 파일 확인
		if (!std::filesystem::exists(path))
		{
			DV_CORE_WARN("{:s}는 존재하지 않는 파일입니다.", path);
		}

		std::wstring filePath(path.begin(), path.end());
		DirectX::ScratchImage image;

		HRESULT hResult;

		// 흐음... 이게 문제였다니...
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
			DV_CORE_WARN("ShaderResourceView 생성에 실패하였습니다.");
			return;
		}
	}
}

// Scene이 있다면
// Enigne의 Backbuffer에서 rtv를 얻어와 그린다.
void ScenePanel::renderWindow()
{
	if (!m_pRenderer)
		return;

	// 크기를 계산한다. => 잘 모르겠다.
	auto width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	// renderer의 viewport를 설정한다.
	// Editor용 Camera도 함께 설정??

	// 크기가 변경되었다면 renderer의 해상도를 변경한다.
	m_pRenderer->SetResolution(static_cast<unsigned int>(width), static_cast<unsigned int>(height));

	// image를 가져와 그린다.
	ImGui::Image(
		m_pSample,	// shader resource view다...
		ImVec2(width, height),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImColor(255, 255, 255, 255),
		ImColor(50, 127, 166, 255)
	);
}
