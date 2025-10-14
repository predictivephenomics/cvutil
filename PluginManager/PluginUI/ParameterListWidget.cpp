/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: ParameterListWidget.h

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

This file is part of Computer Vision UTILity toolkit (cvutil)

cvutil is free software. You can redistribute it and/or modify
it as permissible under the terms of the Noble General Public
License as published by the Noble Research Institute, LLC. This
license is available at the following link.

cvutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Noble General Public License for more details.

You should have received a copy of the Noble General Public License
along with cvutil.  If not, see <https://github.com/noble-research-institute/cvutil/blob/master/LICENSE>.
*/

#include "ParameterListWidget.h"

using namespace std;

ParameterListWidget::ParameterListWidget(IPlugin *_plugin, QWidget *parent) :
    QWidget(parent)
{
    plugin = _plugin;
    //mapper = new QDataWidgetMapper();
    auto params = plugin->getParameters();
    //model = new ParameterListModel(params);
    //mapper->setModel(model);
    int uiwidth = 70;

    BooleanParameter *bp = nullptr;
    IntegerParameter *ip = nullptr;
    FloatParameter *fp = nullptr;
    IntegerRangeParameter *irp = nullptr;
    FloatRangeParameter *frp = nullptr;
    IntegerSpanParameter *ilrp = nullptr;
    ItemsParameter *itp = nullptr;
    vector<string> items;

    WrappableCheckBox *c = nullptr;
    QSpinBox *s = nullptr;
    QDoubleSpinBox *d = nullptr;
    RangeWidget *irw = nullptr;
    FloatRangeWidget *frw = nullptr;
    SpanSliderWidget *rsw = nullptr;
    QComboBox *cb = nullptr;

    QLabel *lb = nullptr;
    QHBoxLayout *hl = nullptr;
    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);

    for (auto &p : params)
    {
        switch (p->getParameterType())
        {
        case ParameterType::Boolean:
            bp = dynamic_cast<BooleanParameter *>(p);
            c = new WrappableCheckBox(QString::fromStdString(bp->getDisplayText()), this);
            c->setVisible(bp->enabled());

            if (bp->getValue())
                c->setCheckState(Qt::CheckState::Checked);
            //mapper->addMapping(c, 0, "checked");
            connect(c, &WrappableCheckBox::clicked, [c, this](bool k)
            {
                int idx = 0;
                WrappableCheckBox *s = c;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                BooleanParameter *bpr = dynamic_cast<BooleanParameter *>(plugin->getParameters()[idx]);
                bpr->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            vl->addWidget(c);
            displaywidgets.push_back({ c });
            controls.push_back(c);
            break;
        case ParameterType::Integer:
            ip = dynamic_cast<IntegerParameter *>(p);
            lb = new QLabel(QString::fromStdString(ip->getDisplayText()), this);
            lb->setWordWrap(true);
            s = new QSpinBox(this);
            s->setMinimum(ip->getMinValue());
            s->setMaximum(ip->getMaxValue());
            s->setValue(ip->getValue());

            lb->setVisible(ip->enabled());
            s->setVisible(ip->enabled());

            connect(s, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [s, this](int k)
            {
                int idx = 0;
                QObject *obj = sender();
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                IntegerParameter *ipr = dynamic_cast<IntegerParameter *>(plugin->getParameters()[idx]);
                ipr->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(s, 1, "value");
            hl = new QHBoxLayout();
            hl->setContentsMargins(0, 0, 0, 0);
            hl->addWidget(lb);
            hl->addWidget(s, 0, Qt::AlignRight);
            vl->addLayout(hl);
            displaywidgets.push_back({ lb, s });
            controls.push_back(s);
            break;
        case ParameterType::Float:
            fp = dynamic_cast<FloatParameter *>(p);
            lb = new QLabel(QString::fromStdString(fp->getDisplayText()), this);
            lb->setWordWrap(true);
            d = new QDoubleSpinBox(this);
            d->setFixedWidth(uiwidth);
            d->setMinimum(fp->getMinValue());
            d->setMaximum(fp->getMaxValue());
            d->setValue(fp->getValue());
            
            lb->setVisible(fp->enabled());
            d->setVisible(fp->enabled());

            connect(d, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [d, this](double k)
            {
                int idx = 0;
                QDoubleSpinBox *s = d;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                FloatParameter *fpr = dynamic_cast<FloatParameter *>(plugin->getParameters()[idx]);
                fpr->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(d, 1, "value");
            hl = new QHBoxLayout();
            hl->setContentsMargins(0, 0, 0, 0);
            hl->addWidget(lb);
            hl->addWidget(d, 0, Qt::AlignRight);
            vl->addLayout(hl);
            displaywidgets.push_back({ lb, d });
            controls.push_back(d);
            break;
        case ParameterType::IntegerRange:
            irp = dynamic_cast<IntegerRangeParameter *>(p);
            lb = new QLabel(QString::fromStdString(irp->getDisplayText()), this);
            lb->setWordWrap(true);
            irw = new RangeWidget(irp->getMinValue(), irp->getMaxValue(), irp->getValue(), this);
            
            lb->setVisible(irp->enabled());
            irw->setVisible(irp->enabled());

            connect(irw, &RangeWidget::valueChanged, [irw, this](int k)
            {
                int idx = 0;
                RangeWidget *s = irw;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                IntegerRangeParameter *ipar = dynamic_cast<IntegerRangeParameter *>(plugin->getParameters()[idx]);
                ipar->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(irw, 1, "Value");
            vl->addWidget(lb);
            vl->addWidget(irw);
            displaywidgets.push_back({ lb, irw });
            controls.push_back(irw);
            break;
        case ParameterType::FloatRange:
            frp = dynamic_cast<FloatRangeParameter *>(p);
            lb = new QLabel(QString::fromStdString(frp->getDisplayText()), this);
            lb->setWordWrap(true);
            frw = new FloatRangeWidget(frp->getMinValue(), frp->getMaxValue(), frp->getValue(), this);
            
            lb->setVisible(frp->enabled());
            frw->setVisible(frp->enabled());

            connect(frw, &FloatRangeWidget::valueChanged, [frw, this](float k)
            {
                int idx = 0;
                FloatRangeWidget *s = frw;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                FloatRangeParameter *fpar = dynamic_cast<FloatRangeParameter *>(plugin->getParameters()[idx]);
                fpar->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(frw, 1, "Value");
            vl->addWidget(lb);
            vl->addWidget(frw);
            displaywidgets.push_back({ lb, frw });
            controls.push_back(frw);
            break;
        case ParameterType::IntegerSpan:
            ilrp = dynamic_cast<IntegerSpanParameter *>(p);
            lb = new QLabel(QString::fromStdString(ilrp->getDisplayText()), this);
            lb->setWordWrap(true);
            rsw = new SpanSliderWidget(ilrp->getMinValue(), ilrp->getMaxValue(), ilrp->getValue(), ilrp->alt_getValue(), this);
            
            lb->setVisible(ilrp->enabled());
            rsw->setVisible(ilrp->enabled());

            connect(rsw, &SpanSliderWidget::valueChanged, [rsw, this](int k)
            {
                int idx = 0;
                SpanSliderWidget *s = rsw;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                IntegerSpanParameter *ipar = dynamic_cast<IntegerSpanParameter *>(plugin->getParameters()[idx]);
                ipar->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });

            connect(rsw, &SpanSliderWidget::alt_valueChanged, [rsw, this](int k)
            {
                int idx = 0;
                SpanSliderWidget *s = rsw;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                IntegerSpanParameter *ipar = dynamic_cast<IntegerSpanParameter *>(plugin->getParameters()[idx]);
                ipar->alt_setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(irw, 1, "Value");
            vl->addWidget(lb);
            vl->addWidget(rsw);
            displaywidgets.push_back({ lb, rsw });
            controls.push_back(rsw);
            break;
        case ParameterType::Items:
            itp = dynamic_cast<ItemsParameter *>(p);
            lb = new QLabel(QString::fromStdString(itp->getDisplayText()), this);
            lb->setWordWrap(true);
            cb = new QComboBox(this);
            cb->setFixedWidth(130);

            items = itp->getItems();
            for (auto& it : items)
                cb->addItem(QString::fromStdString(it));
            cb->setCurrentIndex(itp->getValue());
            
            lb->setVisible(itp->enabled());
            cb->setVisible(itp->enabled());

            connect(cb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [cb, this](int k)
            {
                int idx = 0;
                QComboBox *s = cb;
                auto params = plugin->getParameters();

                for (idx = 0; idx < controls.size(); idx++)
                    if (controls[idx] == s)
                        break;

                if (idx == controls.size())
                    throw("Index out of range.");

                ItemsParameter *cbp = dynamic_cast<ItemsParameter *>(plugin->getParameters()[idx]);
                cbp->setValue(k);

                for (int i = 0; i < params.size(); i++)
                    for (int j = 0; j < displaywidgets[i].size(); j++)
                        displaywidgets[i][j]->setVisible(dynamic_cast<Parameter *>(params[i])->enabled());
            });
            //mapper->addMapping(frw, 1, "currentIndex");
            hl = new QHBoxLayout();
            hl->setContentsMargins(0, 0, 0, 0);
            hl->addWidget(lb);
            hl->addWidget(cb, 0, Qt::AlignRight);
            vl->addLayout(hl);
            displaywidgets.push_back({ lb, cb });
            controls.push_back(cb);
            break;
        default:
            throw("Unknown parameter type.");
            break;
        }
    }

    this->setLayout(vl);
}

RangeWidget::RangeWidget(int minVal, int maxVal, int val, QWidget * parent) : QWidget(parent)
{
    dec = 1;
    box = new QSpinBox();
    box->setMinimum(minVal);
    box->setMaximum(maxVal);
    box->setValue(val);
    box->setFixedWidth(70);

    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setMinimum(int(minVal * dec));
    slider->setMaximum(int(maxVal * dec));
    slider->setValue(int(val * dec));
    slider->setTickInterval(1);

    connect(slider, &QSlider::valueChanged, [&](int k)
    {
        if (box->value() != k)
        {
            box->setValue(k);
            emit valueChanged(k);
        }
    });

    connect(box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int k)
    {
        if (slider->value() != k)
        {
            slider->setValue(k);
            emit valueChanged(k);
        }
    });
    
    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(slider);
    hl->addWidget(box);
    
    hl->setContentsMargins(0, 0, 0, 0);
    this->setLayout(hl);
}

int RangeWidget::Value() const
{
    return val;
}

void RangeWidget::setValue(int v)
{
    val = v;
}

FloatRangeWidget::FloatRangeWidget(float minVal, float maxVal, float val, QWidget * parent) : QWidget(parent)
{
    dec = 1000;
    box = new QDoubleSpinBox();
    box->setDecimals(3);
    box->setMinimum(minVal);
    box->setMaximum(maxVal);
    box->setValue(val);
    box->setSingleStep(0.1);
    box->setFixedWidth(70);

    slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setMinimum(int(minVal * dec));
    slider->setMaximum(int(maxVal * dec));
    slider->setValue(int(val * dec));
    slider->setTickInterval(1);

    connect(slider, &QSlider::valueChanged, [&](int k)
    {
        if (box->value() != double(k / dec))
        {
            box->setValue(double(k / dec));
            emit valueChanged(float(k / dec));
        }
    });

    connect(box, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [&](double k)
    {
        if (slider->value() != double(k * dec))
        {
            slider->setValue(double(k * dec));
            emit valueChanged(float(k / dec));
        }
    });
    
    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(slider);
    hl->addWidget(box);

    hl->setContentsMargins(0, 0, 0, 0);
    this->setLayout(hl);
}

float FloatRangeWidget::Value() const
{
    return val;
}

void FloatRangeWidget::setValue(float v)
{
    val = v;
}

SpanSliderWidget::SpanSliderWidget(int minVal, int maxVal, int minhandle, int maxhandle, QWidget *parent) : QWidget(parent)
{
    dec = 1;
    boxlo = new QSpinBox();
    boxlo->setMinimum(minVal);
    boxlo->setMaximum(maxVal);
    boxlo->setValue(minhandle);
    boxlo->setFixedWidth(40);
    boxhi = new QSpinBox();
    boxhi->setMinimum(minVal);
    boxhi->setMaximum(maxVal);
    boxhi->setValue(maxhandle);
    boxhi->setFixedWidth(40);

    slider = new QxtSpanSlider(Qt::Horizontal);
    slider->setMinimum(int(minVal * dec));
    slider->setMaximum(int(maxVal * dec));
    slider->setLowerPosition(int(minhandle * dec));
    slider->setUpperPosition(int(maxhandle * dec));
    slider->setHandleMovementMode(QxtSpanSlider::HandleMovementMode::NoCrossing);
    slider->setTickInterval(1);

    connect(slider, &QxtSpanSlider::lowerPositionChanged, [&](int k)
    {
        if (boxlo->value() != k)
        {
            boxlo->setValue(k);
            emit valueChanged(k);
        }
    });

    connect(boxlo, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int k)
    {
        if (slider->lowerPosition() != k)
        {
            slider->setLowerPosition(k);
            emit valueChanged(k);
        }
    });

    connect(slider, &QxtSpanSlider::upperPositionChanged, [&](int k)
    {
        if (boxhi->value() != k)
        {
            boxhi->setValue(k);
            emit alt_valueChanged(k);
        }
    });

    connect(boxhi, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int k)
    {
        if (slider->upperPosition() != k)
        {
            slider->setUpperPosition(k);
            emit alt_valueChanged(k);
        }
    });

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(boxlo);
    hl->addWidget(slider);
    hl->addWidget(boxhi);

    hl->setContentsMargins(0, 0, 0, 0);
    this->setLayout(hl);
}

int SpanSliderWidget::Value() const
{
    return val;
}

void SpanSliderWidget::setValue(int v)
{
    val = v;
}

int SpanSliderWidget::alt_Value() const
{
    return alt_val;
}

void SpanSliderWidget::alt_setValue(int v)
{
    alt_val = v;
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f) : QLabel(text, parent, f)
{ }

void ClickableLabel::mouseReleaseEvent(QMouseEvent * event)
{
    emit clicked();
}

WrappableCheckBox::WrappableCheckBox(const QString &text, QWidget *parent) : QWidget(parent)
{
    cb = new QCheckBox();
    cb->setFixedWidth(20);
    lbl = new ClickableLabel(text);
    lbl->setWordWrap(true);

    hl = new QHBoxLayout();
    hl->setContentsMargins(0, 0, 0, 0);
    hl->addWidget(cb);
    hl->addWidget(lbl);

    connect(cb, &QCheckBox::clicked, [&](bool checked = false)
    {
        emit clicked(checked);
    });

    connect(lbl, &ClickableLabel::clicked, [&]()
    {
        cb->setCheckState((cb->checkState() == Qt::CheckState::Checked)
            ? Qt::CheckState::Unchecked
            : Qt::CheckState::Checked);
        emit clicked((cb->checkState() == Qt::CheckState::Checked));
    });

    this->setLayout(hl);
}
