#pragma once
#include <iostream>

class Panel
{
public:
	Panel(const std::string& name) : m_PanelName(name) {}
	virtual ~Panel() = default;

	virtual void OnRender() = 0;

	const std::string& GetPanelName() const { return m_PanelName; }
	void SetPanelName(const std::string& name) { m_PanelName = name; }

private:
	std::string m_PanelName;
};