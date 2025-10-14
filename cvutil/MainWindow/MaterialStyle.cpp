/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: MaterialStyle.cpp

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

This file is part of Computer Vision UTILity toolkit (cvutil)

cvutil is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cvutil is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cvutil; see the file COPYING.  If not, see
<https://www.gnu.org/licenses/>.
*/

#include "MaterialStyle.h"
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QStyleOption>

MaterialStyle::MaterialStyle(QStyle* style)
    : QProxyStyle(style)
{
}

void MaterialStyle::polish(QPalette& palette)
{
    /*palette.setColor(QPalette::Window, QColor(0xFA, 0xFA, 0xFA));
    palette.setColor(QPalette::WindowText, QColor(0x21, 0x21, 0x21));
    palette.setColor(QPalette::Base, QColor(0xFF, 0xFF, 0xFF));
    palette.setColor(QPalette::AlternateBase, QColor(0xF5, 0xF5, 0xF5));
    palette.setColor(QPalette::ToolTipBase, QColor(0xFF, 0xFF, 0xCC));
    palette.setColor(QPalette::ToolTipText, QColor(0x21, 0x21, 0x21));
    palette.setColor(QPalette::Text, QColor(0x21, 0x21, 0x21));
    palette.setColor(QPalette::Button, QColor(0xFF, 0xFF, 0xFF));
    palette.setColor(QPalette::ButtonText, QColor(0x21, 0x21, 0x21));
    palette.setColor(QPalette::BrightText, QColor(0xFF, 0x00, 0x00));
    palette.setColor(QPalette::Highlight, QColor(0x64, 0xB5, 0xF6));
    palette.setColor(QPalette::HighlightedText, QColor(0xFF, 0xFF, 0xFF));*/

    palette.setColor(QPalette::Window, QColor(0x12, 0x12, 0x12)); // Dark background
    palette.setColor(QPalette::WindowText, QColor(0xE0, 0xE0, 0xE0)); // Light text
    palette.setColor(QPalette::Base, QColor(0x33, 0x33, 0x33)); // Darker base
    palette.setColor(QPalette::AlternateBase, QColor(0x42, 0x42, 0x42)); // Alternate base
    palette.setColor(QPalette::ToolTipBase, QColor(0xFF, 0xFF, 0xCC)); // Tooltip base
    palette.setColor(QPalette::ToolTipText, QColor(0x21, 0x21, 0x21)); // Tooltip text
    palette.setColor(QPalette::Text, QColor(0xE0, 0xE0, 0xE0)); // Light text
    palette.setColor(QPalette::Button, QColor(0x33, 0x33, 0x33)); // Dark button
    palette.setColor(QPalette::ButtonText, QColor(0xE0, 0xE0, 0xE0)); // Light button text
    palette.setColor(QPalette::BrightText, QColor(0xFF, 0x00, 0x00)); // Bright text
    palette.setColor(QPalette::Highlight, QColor(0x64, 0xB5, 0xF6)); // Highlight
    palette.setColor(QPalette::HighlightedText, QColor(0xFF, 0xFF, 0xFF)); // Highlighted text
}

void MaterialStyle::polish(QWidget* widget)
{
    QProxyStyle::polish(widget);
}

void MaterialStyle::unpolish(QWidget* widget)
{
    QProxyStyle::unpolish(widget);
}

int MaterialStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    return QProxyStyle::pixelMetric(metric, option, widget);
}

void MaterialStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void MaterialStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    QProxyStyle::drawControl(element, option, painter, widget);
}
