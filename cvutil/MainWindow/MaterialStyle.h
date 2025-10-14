/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: MaterialStyle.h

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

#pragma once
#ifndef MATERIALSTYLE_H
#define MATERIALSTYLE_H

#include <QtWidgets/QProxyStyle>
#include <QtGui/QPalette>

class MaterialStyle : public QProxyStyle
{
    Q_OBJECT;

public:
    MaterialStyle(QStyle* style = nullptr);

    void polish(QPalette& palette) override;
    void polish(QWidget* widget) override;
    void unpolish(QWidget* widget) override;

    int pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const override;
};

#endif // MATERIALSTYLE_H
