/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <LibGUI/BoxLayout.h>
#include <LibGUI/Label.h>
#include <LibGUI/Painter.h>
#include <LibGUI/ResizeCorner.h>
#include <LibGUI/StatusBar.h>
#include <LibGUI/Window.h>
#include <LibGfx/Palette.h>
#include <LibGfx/StylePainter.h>

REGISTER_WIDGET(GUI, StatusBar)

namespace GUI {

StatusBar::StatusBar(int label_count)
{
    set_fixed_height(18);
    set_layout<HorizontalBoxLayout>();
    layout()->set_margins({ 0, 0, 0, 0 });
    layout()->set_spacing(2);

    if (label_count > 0) {
        for (auto i = 0; i < label_count; i++)
            m_labels.append(create_label());

        m_corner = add<ResizeCorner>();
    }

    REGISTER_STRING_PROPERTY("text", text, set_text);
    REGISTER_INT_PROPERTY("label_count", label_count, set_label_count);
}

StatusBar::~StatusBar()
{
}

NonnullRefPtr<Label> StatusBar::create_label()
{
    auto& label = add<Label>();
    label.set_frame_shadow(Gfx::FrameShadow::Sunken);
    label.set_frame_shape(Gfx::FrameShape::Panel);
    label.set_frame_thickness(1);
    label.set_text_alignment(Gfx::TextAlignment::CenterLeft);
    return label;
}

void StatusBar::set_text(const StringView& text)
{
    if (m_labels.is_empty())
        dbgln("set_text() called on a statusbar with no labels. Ensure label_count is set in the constructor or the GML file.");
    m_labels.first().set_text(text);
}

String StatusBar::text() const
{
    if (m_labels.is_empty())
        dbgln("text() called on a statusbar with no labels. Ensure label_count is set in the constructor or the GML file.");
    return m_labels.first().text();
}

void StatusBar::set_text(int index, const StringView& text)
{
    if (m_labels.is_empty())
        dbgln("set_text() called on a statusbar with no labels. Ensure label_count is set in the constructor or the GML file.");
    m_labels.at(index).set_text(text);
}

String StatusBar::text(int index) const
{
    if (m_labels.is_empty())
        dbgln("text() called on a statusbar with no labels. Ensure label_count is set in the constructor or the GML file.");
    return m_labels.at(index).text();
}

void StatusBar::paint_event(PaintEvent& event)
{
    Painter painter(*this);
    painter.add_clip_rect(event.rect());
    painter.fill_rect(rect(), palette().button());
}

void StatusBar::resize_event(ResizeEvent& event)
{
    if (window())
        m_corner->set_visible(window()->is_maximized() ? false : true);

    Widget::resize_event(event);
}

void StatusBar::set_label_count(int label_count)
{
    ASSERT(m_labels.is_empty());
    m_label_count = label_count;
    for (auto i = 0; i < label_count; ++i) {
        m_labels.append(create_label());
    }
    m_corner = add<ResizeCorner>();
}

NonnullRefPtr<Label> StatusBar::label(int index) const
{
    if (m_labels.is_empty())
        dbgln("label() called on a statusbar with no labels. Ensure label_count is set in the constructor or the GML file.");
    return m_labels.at(index);
}

}
