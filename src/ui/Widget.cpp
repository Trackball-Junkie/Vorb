#include "stdafx.h"
#include "UI/Widget.h"
#include "UI/InputDispatcher.h"
#include "UI/UIRenderer.h"

vui::Widget::Widget() : IWidgetContainer() {
    enable();
    m_anchor = {};
}

vui::Widget::Widget(const nString& name, const f32v4& destRect /*= f32v4(0)*/) : IWidgetContainer(name, destRect) {
    enable();
}

vui::Widget::Widget(IWidgetContainer* parent, const nString& name, const f32v4& destRect /*= f32v4(0)*/) : Widget(name, destRect) {
    parent->addWidget(this);
    m_parent = parent;
}

vui::Widget::~Widget() {
    // Empty
}

void vui::Widget::removeDrawables(UIRenderer* renderer) {
    renderer->remove(this);
}

void vui::Widget::updatePosition() {
    f32v2 newPos = m_relativePosition;
    if (m_parent) {
        newPos += m_parent->getPosition();
    }
    m_position = newPos;

    // Update child positions
    for (auto& w : m_widgets) {
        w->updatePosition();
    }
}

void vorb::ui::Widget::setAnchor(const AnchorStyle& anchor) {
    m_anchor = anchor;
}

void vorb::ui::Widget::setDock(const DockStyle& dock) {
    if (m_parent) {
        m_parent->setChildDock(this, dock);
    } else {
        m_dock = dock;
    }
}
