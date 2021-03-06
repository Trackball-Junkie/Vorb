//
// ViewScriptEnvironment.h
// Vorb Engine
//
// Created by Matthew Marshall on 11 Nov 2018
// Based on Ben Arnold's FormScriptEnvironment.
// Copyright 2014 Regrowth Studios
// MIT License
//

/*! \file ViewScriptEnvironment.h
* @brief Manages the script environment of a view.
*/

#pragma once

#ifndef Vorb_ViewScriptEnvironment_h__
//! @cond DOXY_SHOW_HEADER_GUARDS
#define Vorb_ViewScriptEnvironment_h__
//! @endcond

#ifndef VORB_USING_PCH
#include "Vorb/types.h"
#endif // !VORB_USING_PCH

#include "Vorb/ui/script/lua/ValueMediator.h"

#include "Vorb/io/File.h"
#include "Vorb/script/IEnvironment.hpp"
#include "Vorb/ui/widgets/Button.h"
#include "Vorb/ui/script/ButtonScriptFuncs.h"
#include "Vorb/ui/widgets/CheckBox.h"
#include "Vorb/ui/script/CheckBoxScriptFuncs.h"
#include "Vorb/ui/widgets/ComboBox.h"
#include "Vorb/ui/script/ComboBoxScriptFuncs.h"
#include "Vorb/ui/GameWindow.h"
#include "Vorb/ui/script/GameWindowScriptFuncs.h"
#include "Vorb/ui/script/GraphicsScriptFuncs.h"
#include "Vorb/ui/widgets/Label.h"
#include "Vorb/ui/script/LabelScriptFuncs.h"
#include "Vorb/ui/widgets/Panel.h"
#include "Vorb/ui/script/PanelScriptFuncs.h"
#include "Vorb/ui/widgets/Slider.h"
#include "Vorb/ui/script/SliderScriptFuncs.h"
#include "Vorb/ui/script/TextWidgetScriptFuncs.h"
#include "Vorb/ui/widgets/Viewport.h"
#include "Vorb/ui/script/ViewportScriptFuncs.h"
#include "Vorb/ui/script/WidgetScriptFuncs.h"
#include "Vorb/ui/widgets/WidgetList.h"
#include "Vorb/ui/script/WidgetListScriptFuncs.h"

namespace vorb {
    namespace ui {
        // TODO(Matthew): Provide function for loading specified fonts via Lua.
        //                  From a brief look inside SpriteFont we want to revisit and refactor that to complete this.
        // TODO(Matthew): Implement update mechanism for script env.
        template <typename ScriptEnvironmentImpl>
        class ViewScriptEnvironment {
            using ScriptEnv = vscript::IEnvironment<ScriptEnvironmentImpl>;
            using IWidgets  = std::vector<IWidget*>;
        public:
            ViewScriptEnvironment();
            ~ViewScriptEnvironment();

            void init(Viewport* viewport, const GameWindow* window);
            void dispose();

            bool run(const vio::Path& filepath);

            ScriptEnv* getEnv() { return m_env; }
        protected:
            void prepareScriptEnv();

            void registerFuncs();
            void registerConsts();

                vui::Button* makeButton     (IWidget* parent, nString name, f32v4 dimensions);
              vui::CheckBox* makeCheckBox   (IWidget* parent, nString name, f32v4 dimensions);
              vui::ComboBox* makeComboBox   (IWidget* parent, nString name, f32v4 dimensions);
                 vui::Label* makeLabel      (IWidget* parent, nString name, f32v4 dimensions);
                 vui::Panel* makePanel      (IWidget* parent, nString name, f32v4 dimensions);
                vui::Slider* makeSlider     (IWidget* parent, nString name, f32v4 dimensions);
            vui::WidgetList* makeWidgetList (IWidget* parent, nString name, f32v4 dimensions);

            void destroyWidget(IWidget* widget);

            const GameWindow* m_window;

            Viewport*   m_viewport;
            ScriptEnv*  m_env;
            IWidgets    m_widgets;
        };
    }
}
namespace vui = vorb::ui;

template <typename ScriptEnvironmentImpl>
vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::ViewScriptEnvironment() :
    m_viewport(nullptr),
    m_env(nullptr) {
    // Empty
}

template <typename ScriptEnvironmentImpl>
vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::~ViewScriptEnvironment() {
    // Empty
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::init(Viewport* viewport, const GameWindow* window) {
    m_window   = window;
    m_viewport = viewport;
    m_env      = new ScriptEnvironmentImpl();
    m_env->init();

    prepareScriptEnv();
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::dispose() {
    for (auto& widget : m_widgets) {
        delete widget;
    }
    IWidgets().swap(m_widgets);

    m_env->dispose();

    delete static_cast<ScriptEnvironmentImpl*>(m_env);

    m_env      = nullptr;
    m_window   = nullptr;
    m_viewport = nullptr;
}

template <typename ScriptEnvironmentImpl>
bool vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::run(const vio::Path& filepath) {
    return m_env->run(filepath);
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::prepareScriptEnv() {
    registerFuncs();

    registerConsts();
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::registerFuncs() {
    // Set up creation functions for extending the view.
    m_env->setNamespaces("UI", "View");
    m_env->addCDelegate("makeButton",     makeDelegate(this, &ViewScriptEnvironment::makeButton));
    m_env->addCDelegate("makeCheckBox",   makeDelegate(this, &ViewScriptEnvironment::makeCheckBox));
    m_env->addCDelegate("makeComboBox",   makeDelegate(this, &ViewScriptEnvironment::makeComboBox));
    m_env->addCDelegate("makeLabel",      makeDelegate(this, &ViewScriptEnvironment::makeLabel));
    m_env->addCDelegate("makePanel",      makeDelegate(this, &ViewScriptEnvironment::makePanel));
    m_env->addCDelegate("makeSlider",     makeDelegate(this, &ViewScriptEnvironment::makeSlider));
    m_env->addCDelegate("makeWidgetList", makeDelegate(this, &ViewScriptEnvironment::makeWidgetList));
    m_env->addCDelegate("destroyWidget",  makeDelegate(this, &ViewScriptEnvironment::destroyWidget));
    m_env->setNamespaces();

    ButtonScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("Button", m_env);

    CheckBoxScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("CheckBox", m_env);

    ComboBoxScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("ComboBox", m_env);

    GameWindowScriptFuncs::registerFuncs<ScriptEnvironmentImpl>(m_env, m_window);

    // TODO(Matthew): Provide actual texture cache object!
    GraphicsScriptFuncs::registerFuncs<ScriptEnvironmentImpl>(m_env, nullptr);

    LabelScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("Label", m_env);

    PanelScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("Panel", m_env);

    SliderScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("Slider", m_env);

    ViewportScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("Viewport", m_env);

    WidgetListScriptFuncs::registerFuncs<ScriptEnvironmentImpl>("WidgetList", m_env);
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::registerConsts() {
    m_env->setNamespaces("UI", "View");
    m_env->addValue("port", m_viewport);
    m_env->setNamespaces();

    ButtonScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    CheckBoxScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    ComboBoxScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    GameWindowScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    GraphicsScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    LabelScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    PanelScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    SliderScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    ViewportScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    WidgetScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);

    WidgetListScriptFuncs::registerConsts<ScriptEnvironmentImpl>(m_env);
}

template <typename ScriptEnvironmentImpl>
vui::Button* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeButton(IWidget* parent, nString name, f32v4 dimensions) {
    Button* button = new Button();

    button->init(parent, name, dimensions);

    m_widgets.push_back(button);

    return button;
}

template <typename ScriptEnvironmentImpl>
vui::CheckBox* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeCheckBox(IWidget* parent, nString name, f32v4 dimensions) {
    CheckBox* checkBox = new CheckBox();

    checkBox->init(parent, name, dimensions);

    m_widgets.push_back(checkBox);

    return checkBox;
}

template <typename ScriptEnvironmentImpl>
vui::ComboBox* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeComboBox(IWidget* parent, nString name, f32v4 dimensions) {
    ComboBox* comboBox = new ComboBox();

    comboBox->init(parent, name, dimensions);

    m_widgets.push_back(comboBox);

    return comboBox;
}

template <typename ScriptEnvironmentImpl>
vui::Label* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeLabel(IWidget* parent, nString name, f32v4 dimensions) {
    Label* label = new Label();

    label->init(parent, name, dimensions);

    m_widgets.push_back(label);

    return label;
}

template <typename ScriptEnvironmentImpl>
vui::Panel* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makePanel(IWidget* parent, nString name, f32v4 dimensions) {
    Panel* panel = new Panel();

    panel->init(parent, name, dimensions);

    m_widgets.push_back(panel);

    return panel;
}

template <typename ScriptEnvironmentImpl>
vui::Slider* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeSlider(IWidget* parent, nString name, f32v4 dimensions) {
    Slider* slider = new Slider();

    slider->init(parent, name, dimensions);

    m_widgets.push_back(slider);

    return slider;
}

template <typename ScriptEnvironmentImpl>
vui::WidgetList* vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::makeWidgetList(IWidget* parent, nString name, f32v4 dimensions) {
    WidgetList* widgetList = new WidgetList();

    widgetList->init(parent, name, dimensions);

    m_widgets.push_back(widgetList);

    return widgetList;
}

template <typename ScriptEnvironmentImpl>
void vui::ViewScriptEnvironment<ScriptEnvironmentImpl>::destroyWidget(IWidget* widget) {
    // By default this function acts to just dispose the widget, unless we also manage it.
    widget->dispose();

    // If we manage this widget, clear up memory.
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        m_widgets.erase(it);
        delete widget;
    }
}

#endif // !Vorb_ViewScriptEnvironment_h__
