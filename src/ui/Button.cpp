#include "stdafx.h"
#include "ui/Button.h"
#include "ui/MouseInputDispatcher.h"
#include "ui/UIRenderer.h"

vui::Button::Button() : Widget() {
    updateColor();
}

vui::Button::Button(const nString& name, const f32v4& destRect /*= f32v4(0)*/) : Button() {
    m_name = name;
    setDestRect(destRect);
    m_drawableRect.setPosition(getPosition());
    m_drawableRect.setDimensions(getDimensions());
    updateTextPosition();
}

vui::Button::Button(Widget* parent, const nString& name, const f32v4& destRect /*= f32v4(0)*/) : Button(name, destRect) {
    parent->addWidget(this);
    m_parent = parent;
}

vui::Button::~Button() {
    // Empty
}

void vui::Button::addDrawables(UIRenderer* renderer) {
    // Make copies
    m_drawnText = m_drawableText;
    m_drawnRect = m_drawableRect;
    // Use renderer default font if we dont have a font
    m_defaultFont = renderer->getDefaultFont();
    if (!m_drawnText.getFont()) m_drawnText.setFont(m_defaultFont);

    // Add the rect
    renderer->add(this,
                  makeDelegate(m_drawnRect, &DrawableRect::draw),
                  makeDelegate(*this, &Button::refreshDrawables));

    
    // Add the text 
    renderer->add(this,
                  makeDelegate(m_drawnText, &DrawableText::draw),
                  makeDelegate(*this, &Button::refreshDrawables));  
}

void vui::Button::removeDrawables(UIRenderer* renderer) {
    renderer->remove(this);
}

void vui::Button::updatePosition() {
    Widget::updatePosition();
    m_drawableRect.setPosition(getPosition());
    m_drawableRect.setDimensions(getDimensions());
    updateTextPosition();
}

void vui::Button::setDestRect(const f32v4& destRect) {
    vui::Widget::setDestRect(destRect);
    m_drawableRect.setPosition(getPosition());
    m_drawableRect.setDimensions(getDimensions());
}

void vui::Button::setDimensions(const f32v2& dimensions) {
    Widget::setDimensions(dimensions);
    m_drawableRect.setDimensions(dimensions);
    updateTextPosition();
}

void vui::Button::setFont(const vorb::graphics::SpriteFont* font) {
    m_font = font;
    updateTextPosition();
}

void vui::Button::setHeight(f32 height) {
    Widget::setHeight(height);
    m_drawableRect.setHeight(height);
    updateTextPosition();
}

void vui::Button::setPosition(const f32v2& position) {
    Widget::setPosition(position);
    m_drawableRect.setPosition(m_position);
    updateTextPosition();
}

void vui::Button::setTexture(VGTexture texture) {
    m_drawableRect.setTexture(texture);
    refreshDrawables();
}

void vui::Button::setWidth(f32 width) {
    Widget::setWidth(width);
    m_drawableRect.setWidth(width);
    updateTextPosition();
}

void vui::Button::setX(f32 x) {
    Widget::setX(x);
    m_drawableRect.setX(m_position.x);
    updateTextPosition();
}

void vui::Button::setY(f32 y) {
    Widget::setY(y);
    m_drawableRect.setX(m_position.y);
    updateTextPosition();
}

void vui::Button::setBackColor(const color4& color) {
    m_backColor = color;
    updateColor();
}

void vui::Button::setBackHoverColor(const color4& color) {
    m_backHoverColor = color;
    updateColor();
}

void vui::Button::setText(const nString& text) {
    m_drawableText.setText(text);
    updateTextPosition();
}

void vui::Button::setTextColor(const color4& color) {
    m_textColor = color;
    updateColor();
}

void vui::Button::setTextHoverColor(const color4& color) {
    m_textHoverColor = color;
    updateColor();
}

void vui::Button::setTextAlign(vg::TextAlign textAlign) {
    m_drawableText.setTextAlign(textAlign);
    updateTextPosition();
}

void vui::Button::setTextScale(const f32v2& textScale) {
    m_drawableText.setTextScale(textScale);
    updateTextPosition();
}

void vui::Button::updateColor() {
    if (m_isMouseIn) {
        m_drawableRect.setColor(m_backHoverColor);
        m_drawableText.setColor(m_textHoverColor);
    } else {
        m_drawableRect.setColor(m_backColor);
        m_drawableText.setColor(m_textColor);
    }
    refreshDrawables();
}

void vui::Button::updateTextPosition() {
    const f32v2& dims = getDimensions();
    const f32v2& pos = getPosition();
    const vg::TextAlign& textAlign = getTextAlign();
    m_drawableText.setClipRect(f32v4(m_position.x, m_position.y, m_dimensions.x, m_dimensions.y));

    // TODO(Ben): Padding
    switch (textAlign) {
        case vg::TextAlign::LEFT:
            m_drawableText.setPosition(pos + f32v2(0.0f, dims.y / 2.0f));
            break;
        case vg::TextAlign::TOP_LEFT:
            m_drawableText.setPosition(pos);
            break;
        case vg::TextAlign::TOP:
            m_drawableText.setPosition(pos + f32v2(dims.x / 2.0f, 0.0f));
            break;
        case vg::TextAlign::TOP_RIGHT:
            m_drawableText.setPosition(pos + f32v2(dims.x, 0.0f));
            break;
        case vg::TextAlign::RIGHT:
            m_drawableText.setPosition(pos + f32v2(dims.x, dims.y / 2.0f));
            break;
        case vg::TextAlign::BOTTOM_RIGHT:
            m_drawableText.setPosition(pos + f32v2(dims.x, dims.y));
            break;
        case vg::TextAlign::BOTTOM:
            m_drawableText.setPosition(pos + f32v2(dims.x / 2.0f, dims.y));
            break;
        case vg::TextAlign::BOTTOM_LEFT:
            m_drawableText.setPosition(pos + f32v2(0.0f, dims.y));
            break;
        case vg::TextAlign::CENTER:
            m_drawableText.setPosition(pos + dims / 2.0f);
            break;
    }
    refreshDrawables();
}

void vui::Button::refreshDrawables() {

    // Use renderer default font if we don't have a font
    if (!m_drawableText.getFont()) {
        m_drawableText.setFont(m_defaultFont);
        m_drawnText = m_drawableText;
        m_drawableText.setFont(nullptr);
    } else {
        m_drawnText = m_drawableText;
    }
    
    m_drawnRect = m_drawableRect;
}

void vui::Button::onMouseMove(Sender s, const MouseMotionEvent& e) {
    if (!m_isEnabled) return;
    if (isInBounds((f32)e.x, (f32)e.y)) {
        if (!m_isMouseIn) {
            m_isMouseIn = true;
            MouseEnter(e);
            updateColor();
        }
        MouseMove(e);
    } else {        
        if (m_isMouseIn) {
            m_isMouseIn = false;
            MouseLeave(e);
            updateColor();
        }
    }
}

void vui::Button::onMouseFocusLost(Sender s, const MouseEvent& e) {
    if (!m_isEnabled) return;
    if (m_isMouseIn) {
        m_isMouseIn = false;
        MouseMotionEvent ev;
        ev.x = e.x;
        ev.y = e.y;
        MouseLeave(ev);
        updateColor();
    }
}
