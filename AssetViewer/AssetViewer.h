#pragma once
#include "Dive.h"
#include "AssetImporter.h"

#include <imgui-docking/imgui.h>
#include <imgui-docking/imgui_impl_win32.h>
#include <imgui-docking/imgui_impl_dx11.h>
#include <Imgui-docking/imgui_internal.h>
#include <imgui-docking/imgui_stdlib.h>

class AssetViewer : public Dive::Application
{
public:
	AssetViewer();
	~AssetViewer() override;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnUpdate(const Dive::Event& e);
	void OnPostUpdate(const Dive::Event& e);
	void OnWindowEvent(const Dive::Event& e);

private:
	void initializeImGui();
	void drawTree(Dive::GameObject* pNode);

private:
	AssetImporter m_AssetImporter;

	Dive::GameObject* m_pCamera;
	Dive::GameObject* m_pLoadedModel;
	Dive::GameObject* m_pSelectedNode;
	
	std::vector<Dive::IMeshRenderer*> m_MeshRenderers;

	// menu
	ImVec4 m_ClearColor;

	uint32_t m_Fps;

	float m_CameraMoveSpeed;
	float m_CameraRotationSpeed;
};