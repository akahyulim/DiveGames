#pragma once
#include <iostream>

class ModelViewer;

class Panel
{
public:
	Panel(ModelViewer* pModelViewer);
	virtual ~Panel() = default;

	virtual void OnRender() = 0;

	const std::string& GetPanelName() const { return m_PanelName; }

protected:
	std::string m_PanelName		= "Panel";
	ModelViewer* m_pModelViewer = nullptr;
};