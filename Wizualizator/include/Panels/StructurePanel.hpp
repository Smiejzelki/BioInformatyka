#pragma once
#include "PanelBase.hpp"

class Camera;
class Canvas;

class StructurePanel final : public PanelBase
{
public:
	StructurePanel() noexcept;
	virtual ~StructurePanel() = default;

	virtual void OnGUIRender() override final;
	virtual void OnEvent(const Event& event) override final;
private:
	void RedrawCanvas();
private:
	std::unique_ptr<Camera> m_Camera;
	std::unique_ptr<Canvas> m_Canvas;

	bool m_IsPanelHovered;
	bool m_IsPanelDragged;
	bool m_ShouldRedraw;

	float m_PHLevel;
};