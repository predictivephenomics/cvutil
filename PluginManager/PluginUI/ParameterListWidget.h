/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: ParameterListWidget.h

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

#ifndef PARAMETERLISTWIDGET_H
#define PARAMETERLISTWIDGET_H

//#include "ImageProcessor.h"
//#include "ParameterListModel.h"
#include <QtWidgets/QtWidgets>

#include "../include/PluginInterfaces.h"
#include "qxtspanslider.h"

class RangeWidget : public QWidget
{
    Q_OBJECT;
    Q_PROPERTY(int Value READ Value WRITE setValue NOTIFY valueChanged USER true);

public:
    RangeWidget(int minVal, int maxVal, int val, QWidget *parent = nullptr);

    int Value() const;
    void setValue(int v);
    
signals:
    void valueChanged(int v);

private:
    QSlider *slider;
    QSpinBox *box;
    
    int dec = 0;
    int val = 0;
};

class FloatRangeWidget : public QWidget
{
    Q_OBJECT;
    Q_PROPERTY(float Value READ Value WRITE setValue NOTIFY valueChanged USER true);

public:
    FloatRangeWidget(float minVal, float maxVal, float val, QWidget *parent = nullptr);

    float Value() const;
    void setValue(float v);

signals:
    void valueChanged(float v);

private:
    QSlider *slider;
    QDoubleSpinBox *box;

    int dec = 0;
    float val = 0;
};

class SpanSliderWidget : public QWidget
{
    Q_OBJECT;
    Q_PROPERTY(float Value READ Value WRITE setValue NOTIFY valueChanged USER true);

public:
    SpanSliderWidget(int minVal, int maxVal, int minhandle, int maxhandle, QWidget *parent = nullptr);

    int Value() const;
    void setValue(int v);
    int alt_Value() const;
    void alt_setValue(int v);

signals:
    void valueChanged(int v);
    void alt_valueChanged(int v);

private:
    QxtSpanSlider *slider;
    QSpinBox *boxlo, *boxhi;

    int minHandle = 0, maxHandle = 0;

    int dec = 0;
    float val = 0, alt_val = 0;
};

class ClickableLabel : public QLabel
{
    Q_OBJECT;

public:
    ClickableLabel(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

class WrappableCheckBox : public QWidget
{
    Q_OBJECT;

public:
    WrappableCheckBox(const QString &text, QWidget *parent = Q_NULLPTR);
    
    Qt::CheckState checkState() const { return cb->checkState(); };
    bool isTristate() const { return cb->isTristate(); }
    void setCheckState(Qt::CheckState state) { cb->setCheckState(state); };
    void setTristate(bool y = true) { cb->setTristate(y); };

    QString text() const { return lbl->text(); };
    void setText(const QString &txt) { lbl->setText(txt); };
    bool wordWrap() const { return lbl->wordWrap(); };
    void setWordWrap(bool on) { lbl->setWordWrap(on); };

signals:
    void clicked(bool checked = false);

private:
    QHBoxLayout * hl;
    QCheckBox *cb;
    ClickableLabel *lbl;
};

class ParameterListWidget : public QWidget
{
    Q_OBJECT;

public:
    ParameterListWidget(IPlugin *_plugin, QWidget *parent = nullptr);
    
private:
    //QDataWidgetMapper *mapper;
    //ParameterListModel *model;
    IPlugin *plugin;

    std::vector<std::vector<QWidget *>> displaywidgets;
    std::vector<QWidget *> controls;
};


#endif
