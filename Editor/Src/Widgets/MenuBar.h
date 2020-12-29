#pragma once
#include "Widget.h"

class Editor;

class MenuBar : public Widget
{
public:
	MenuBar(Editor* editor);

	void Tick(float deltaTime = 0.0f) override;

	//void SetSelected(Dive::GameObject* pSelected) { m_pSelectedObj = pSelected; }

private:
	void popupAbout();

private:
//	Dive::World* m_pWorld;
//	Dive::GameObject* m_pSelectedObj;
	Scene* m_scene;

	// µÑ ´Ù dialog
	bool m_bShowProjectOpen		= false;
	bool m_bShowSceneOpen		= false;

	bool m_bShowFileDialog		= false;

	bool m_bPopupCreateProject	= false;
	bool m_bPopupOpenProject	= false;
	bool m_bPopupCreateScene	= false;
	bool m_bPopupOpenScene		= false;
	bool m_bPopupAbout			= false;

	std::string m_projectName	= "no name";
	std::string m_sceneName		= "no name";
	bool m_bOpenedProject		= false;
	bool m_bOpenedScene			= false;
};

