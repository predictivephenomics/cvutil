/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: MainWindow.cpp

Authors:
Anand Seethepalli (anand.seethepalli@yahoo.co.in)
Larry York (larry.york@gmail.com)

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

#include "MainWindow.h"
#include "BatchProcessor.h"
#include "logger.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

#include <QtGui/QPainter>
#include <QtCore/QTimer>

#include <algorithm>

using namespace std;
using namespace cv;
//using namespace cvutil;

QTextEdit *MainWindow::logbox = 0;

#ifdef _DEBUG
bool MainWindow::showDebugMessages = true;
#else
bool MainWindow::showDebugMessages = false;
#endif

void GraphicsView::wheelEvent(QWheelEvent *e)
{
    SetScale(scaleval * pow(scalemultiplier, e->delta() / 120.0));
}

void GraphicsView::dragEnterEvent(QDragEnterEvent * event)
{
    /*if (event->mimeData()->hasFormat("text/uri-list"))
    {
    const QMimeData* mimeData = event->mimeData();

    QStringList pathList;
    QList<QUrl> urlList = mimeData->urls();

    if (urlList.size() == 1)
    {
    QString filename = urlList.at(0).toLocalFile();

    if (filename.endsWith(".png", Qt::CaseInsensitive) ||
    filename.endsWith(".jpg", Qt::CaseInsensitive) ||
    filename.endsWith(".jpeg", Qt::CaseInsensitive) ||
    filename.endsWith(".bmp", Qt::CaseInsensitive) ||
    filename.endsWith(".tif", Qt::CaseInsensitive) ||
    filename.endsWith(".tiff", Qt::CaseInsensitive))
    {*/
    event->acceptProposedAction();
    /*       }
    }
    }*/
}

void GraphicsView::dragMoveEvent(QDragMoveEvent * event)
{
    event->acceptProposedAction();
}

void GraphicsView::dropEvent(QDropEvent * event)
{
    emit openFile(event->mimeData()->urls().at(0).toLocalFile());
    event->acceptProposedAction();
}

void GraphicsView::propagatePixelInfo(int rowid, int colid)
{
    emit showPixelInfo(rowid, colid);
}

GraphicsView::GraphicsView()
{
    gscene = new GraphicsScene();
    gscene->setItemIndexMethod(QGraphicsScene::NoIndex);
    gscene->setSceneRect(0, 0, 0, 0);
    gscene->setBackgroundBrush(Qt::gray);
    
    RoiManager *mgr = RoiManager::GetInstance();
    //mgr->setScene(gscene);

    //gscene->installEventFilter(this);
    setScene(gscene);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHint(QPainter::RenderHint::Antialiasing, false);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(1.0), qreal(1.0));
    setMinimumSize(400, 400);
    //setMouseTracking(true);
    setDragMode(QGraphicsView::ScrollHandDrag);

    connect(gscene, &GraphicsScene::showPixelInfo, this, &GraphicsView::propagatePixelInfo);
}

GraphicsView::GraphicsView(QWidget * parent) : QGraphicsView(parent)
{
    gscene = new GraphicsScene();
    gscene->setItemIndexMethod(QGraphicsScene::NoIndex);
    gscene->setSceneRect(0, 0, 1000, 1000);
    gscene->setBackgroundBrush(Qt::gray);
    gscene->installEventFilter(this);
    setScene(gscene);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setRenderHint(QPainter::RenderHint::Antialiasing, false);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(1.0), qreal(1.0));
    setMinimumSize(400, 400);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

double GraphicsView::GetScale()
{
    return scaleval;
}

void GraphicsView::SetScale(double s, bool updateTransform)
{
    if (!enabletransformations)
        return;
    /*QTransform t = transform();
    qreal factor = t.scale(s, s).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    
    scale(s, s);
    QTransform tr = transform();
    scaleval = tr.m11();*/
    RoiManager *mgr = RoiManager::GetInstance();
    
    if (updateTransform)
    {
        s = qBound(0.01, s, 100.0);
        QTransform tr = transform();
        tr.scale(1.0 / scaleval, 1.0 / scaleval);
        tr.scale(s, s);
        this->setTransform(tr);
        //this->setTransform(QTransform(s, tr.m12(), tr.m13(), tr.m21(), s, tr.m23(), tr.m31(), tr.m32(), tr.m33()));
        //this->setTransform(tr);
        //qreal sc = this->GetScale();
        //this->scale(1/sc, 1/sc);
        //this->scale(s, s);
        scaleval = s;
        //gscene->setScale(s);
        mgr->setROIBorderWidth(s);
    }
    else
        //gscene->setScale(s);
        mgr->setROIBorderWidth(s);
}

void GraphicsView::setPixmap(QImage image)
{
    //QTransform tr = transform();
    if (pix == nullptr)
    {
        gscene->clear();
        pix = gscene->addPixmap(QPixmap::fromImage(image));
    }
    else
        pix->setPixmap(QPixmap::fromImage(image));

    gscene->setSceneRect(0, 0, image.width(), image.height());
    setScene(gscene);
}

void GraphicsView::setRotation(int rot)
{
    if (!enabletransformations)
        return;

    int r = rot - rotation;

    while (r < 0)
        r += 360;
    while (r >= 360)
        r -= 360;

    rotation = rot;
    rotate(r);
    gscene->setViewRotation(rotation);
}

int GraphicsView::getRotation()
{
    return rotation;
}

void GraphicsView::setEnabledTransformations(bool on)
{
    enabletransformations = on;
    gscene->setEnabledTransformations(on);
}

void MainWindow::setRotation(int rotation)
{
    view->setRotation(rotation);
}

void MainWindow::logUpdated()
{
    logdocker->raise();
    logbox->verticalScrollBar()->setSliderPosition(
        logbox->verticalScrollBar()->maximum());
}

int MainWindow::getRotation()
{
    return view->getRotation();
}

bool MainWindow::colorInversionEnabled()
{
    return invertColors->isChecked();
}

void MainWindow::setColorInversionEnabled(bool on)
{
    invertColors->setChecked(on);
}

void MainWindow::zoomin()
{
    //area->SetScale(area->GetScale() * area->scalemultiplier);
    view->SetScale(view->GetScale() * view->scalemultiplier);
}

void MainWindow::zoomout()
{
    //area->SetScale(area->GetScale() / area->scalemultiplier);
    view->SetScale(view->GetScale() / view->scalemultiplier);
}

void MainWindow::zoomfit()
{
    view->fitInView(view->scene()->sceneRect(), Qt::KeepAspectRatio);

    QTransform tr = view->transform();
    view->scaleval = sqrt(tr.m11() * tr.m11() + tr.m12() * tr.m12());
    view->SetScale(view->scaleval, false);
    //this->setTransform(QTransform(tr.m11(), tr.m12(), tr.m13(), tr.m21(), tr.m22(), tr.m23(), tr.m31(), tr.m32(), tr.m33()));
}

void MainWindow::displayPixelInfo(int x, int y)
{
    if (img.width() != 0 && y >= 0 && x >= 0 && y < input.rows && x < input.cols)
    {
        Mat out, gr;

        if (input.channels() == 3)
            gr = input;
        else
            cvtColor(input, gr, cv::COLOR_GRAY2RGB);

        if (pfmt == "RGB")
            out = gr;
        else if (pfmt == "HSV")
            cvtColor(gr, out, cv::COLOR_RGB2HSV);
        else if (pfmt == "HLS")
            cvtColor(gr, out, cv::COLOR_RGB2HLS); 
        else if (pfmt == "Lab")
            cvtColor(gr, out, cv::COLOR_RGB2Lab);
        else if (pfmt == "Luv")
            cvtColor(gr, out, cv::COLOR_RGB2Luv);

        qInfo() << "Pixel at (" << y << ", " << x << ") - " << pfmt.toStdString().c_str() << "(" << out.at<Vec3b>(y, x)[0] << ", " << out.at<Vec3b>(y, x)[1] << ", " << out.at<Vec3b>(y, x)[2] << ")";
    }
}

void MainWindow::loadPlugins(IPlugin *plugin)
{
    if (actiondocker->widget() != nullptr)
        actiondocker->widget()->deleteLater();

    if (!plugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        p->Load();
        pwidgets = p->GetPluginUIs();
        setPluginUI();
        actiondocker->setWindowTitle("Plugin settings");
        mainplugin = nullptr;

        /*auto pl = p->GetPlugins();
        for (int i = 0; i < pl.size(); i++)
            connect(dynamic_cast<QObject*>(pl[i]), SIGNAL( updateVisualOutput() ), this, SLOT( setImage() ));*/
    }
    else
    {
        mainplugin = plugin;
        //connect(dynamic_cast<QObject*>(mainplugin), SIGNAL(updateVisualOutput()), this, SLOT(setImage()));
        actiondocker->setWidget(plugin->getPluginUI());
        actiondocker->setWindowTitle("Analysis Options");
        IsCustomWidget = true;
    }
    
    if (PluginManager::GetInstance()->ListNames().size() == 0 && mainplugin == nullptr)
        qCritical() << "No plugins loaded.";
    /*auto plugins = p->GetPlugins();

    for (auto plugin : plugins)
        pwidgets.push_back(new ParameterListWidget(plugin));*/
    reinitfeatureview();
}

void MainWindow::refreshPlugins()
{
    if (actiondocker->widget() != nullptr)
        actiondocker->widget()->deleteLater();

    loadPlugins(mainplugin);
}

void MainWindow::enableROI(bool on)
{
    roienabled = on;
    roipropdocker->setVisible(on);
    roipropdocker->toggleViewAction()->setEnabled(on);
    roitoolbaract->setEnabled(on);
}

void MainWindow::enableActions(bool on)
{
    actionsenabled = on;
    actiondocker->setVisible(on);
    actiondocker->toggleViewAction()->setEnabled(on);
}

void MainWindow::setVisibleROI(bool on)
{
    roipropdocker->setVisible(on);
}

void MainWindow::setVisibleActions(bool on)
{
    actiondocker->setVisible(on);
}

void MainWindow::setAboutText(std::string txt, QPixmap applicationimage, QIcon applicationicon)
{
    abttxt = txt;
    appimage = applicationimage;
    appicon = applicationicon;
}

cv::Mat MainWindow::getLoadedImage()
{
    return input.clone();
}

int MainWindow::getDisplayOutputIndex()
{
    /*IPlugin *pl = nullptr;
    
    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;
    
    if (pl->getDisplayIndex() < output.size())
        return (output[pl->getDisplayIndex()]);
*/
    return displayidx;
}

void MainWindow::setDisplayOutputIndex(int idx)
{
    /*if (displayidx == idx)
        return;*/
    IPlugin *pl;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;

    /*try
    {*/
    output.clear();
    output = pl->getOutputImages();
    /*}
    catch (exception e)
    {
        std::cout << e.what();
    }*/

    if (idx < 0 || idx >= (int(output.size()) + 1))
        return;

    displayidx = idx;

    if (displayidx == 0 && (!input.empty()))
    {
        if (invertColors->isChecked())
            setImage(Scalar::all(255) - input);
        else
            setImage(input);
    }
    else if (!output[displayidx - 1].empty())
        setImage(output[displayidx - 1]);
}

//DisplayMode MainWindow::getDisplayMode()
//{
//    return mode;
//}

void MainWindow::setPluginUI()
{
    pnamelist = new QComboBox();
    PluginManager *p = PluginManager::GetInstance();

    for (auto &pn : p->ListNames())
        pnamelist->addItem(QString::fromStdString(pn));

    if(p->ListNames().size() > curridx)
        pnamelist->setCurrentIndex(curridx);

    vl = new QVBoxLayout();
    //vl->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *hl = new QHBoxLayout();
    hl->setContentsMargins(0, 0, 0, 0);
    QLabel *lb = new QLabel("Algorithm : ");
    hl->addWidget(lb);
    hl->addWidget(pnamelist, 1);
    
    vl->addLayout(hl);
    vl->addSpacing(10);
    if (pwidgets.size() > 0)
        vl->addWidget(pwidgets[0], 0, Qt::Alignment::enum_type::AlignTop);
    
    //execute = new QPushButton("Execute");
    //vl->addWidget(execute, 0, Qt::Alignment::enum_type::AlignRight | Qt::Alignment::enum_type::AlignBottom);

    /*connect(execute, &QPushButton::clicked, [&](bool checked = false)
    {
        processImage();
    });*/

    connect(pnamelist, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int idx) 
    {
        QLayoutItem *item = vl->replaceWidget(pwidgets[curridx], pwidgets[idx]);
        delete item;
        pwidgets[curridx]->close();
        pwidgets[idx]->show();

        curridx = idx;
        this->update();
    });

    IsCustomWidget = false;
    QWidget *w = new QWidget(this);
    w->setLayout(vl);
    actiondocker->setWidget(w);
    actiondocker->update();
}

void MainWindow::setROIUI()
{
#define INIT_SPINBOX(x) \
    (x) = new QSpinBox(); \
    (x)->setMinimum(0); \
    (x)->setMaximum(100); \
    (x)->setValue(0); \
    (x)->setReadOnly(true);

    RoiManager *mgr = RoiManager::GetInstance();

    INIT_SPINBOX(spx);
    INIT_SPINBOX(spy);
    INIT_SPINBOX(spw);
    INIT_SPINBOX(sph);
    
    classname = new QLineEdit();
    roiname = new QLineEdit();

    roilist = new QListWidget();
    roilist->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QLabel *roilistlbl = new QLabel("ROI List");
    QFont fnt = roilistlbl->font();
    fnt.setPointSizeF(fnt.pointSizeF() * 1.3);
    roilistlbl->setFont(fnt);

    // Connect to image change signal
    connect(this, &MainWindow::imageChanged, mgr, [&]()
    {
        RoiManager *mgr = RoiManager::GetInstance();

        auto rois = mgr->getROIs();
        QRectF imgrect = img.rect();

        for (auto&roi : rois)
        {
            QRectF roirect = roi->rect();

            if (roi->rect().height() > img.rect().height() && 
                roi->rect().width() > img.rect().width())
                view->scene()->removeItem(roi);
        }

        mgr->updateBoundaries(view->scene()->width(), view->scene()->height()); 
    });

    connect(mgr, &RoiManager::selectedROIDimensionChanged, this, &MainWindow::updateROIDimensions);
    connect(view->scene(), &GraphicsScene::selectionChanged, this, &MainWindow::updateROIDimensions);

    connect(classname, &QLineEdit::textChanged, [&]()
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        auto si = mgr->getSelectedIndices();

        for (int i = 0; i < si.size(); i++)
            mgr->setClassName(si[i], classname->text().toStdString());

        if (si.size() > 0)
            mgr->setDefaultClassName(classname->text().toStdString());
    });

    connect(roiname, &QLineEdit::textChanged, [&]()
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        auto si = mgr->getSelectedIndices();

        if (si.size() == 1)
        {
            mgr->setROIName(si[0], roiname->text().toStdString());
            roilist->item(si[0])->setText(roiname->text());
        }
    });

    connect(spx, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int nv)
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        mgr->setX(nv, view->scene()->width());
        setROILimits();
    });

    connect(spy, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int nv)
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        mgr->setY(nv, view->scene()->height());
        setROILimits();
    });

    connect(spw, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int nv)
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        mgr->setWidth(nv, view->scene()->width());
        setROILimits();
    });

    connect(sph, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [&](int nv)
    {
        if (updatingROI)
            return;

        RoiManager *mgr = RoiManager::GetInstance();
        mgr->setHeight(nv, view->scene()->height());
        setROILimits();
    });

    connect(roilist, &QListWidget::itemSelectionChanged, [&]()
    {
        if (updatingroilist)
            return;

        QList<QListWidgetItem *> items = roilist->selectedItems();
        RoiManager *mgr = RoiManager::GetInstance();
        uilistselecting = true;
        mgr->clearSelection();
        
        for (int i = 0; i < items.count(); i++)
        {
            mgr->selectROIbyName(items[i]->text().toStdString());
        }

        uilistselecting = false;
    });

    QFormLayout *fl = new QFormLayout();
    fl->addRow(tr("Name:"), roiname);
    fl->addRow(tr("Class:"), classname);
    fl->addRow(tr("X:"), spx);
    fl->addRow(tr("Y:"), spy);
    fl->addRow(tr("Width:"), spw);
    fl->addRow(tr("Height:"), sph);
    
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addLayout(fl);
    vl->addWidget(roilistlbl);
    vl->addWidget(roilist);

    QWidget *w = new QWidget(this);
    w->setLayout(vl);
    roipropdocker->setWidget(w);
#undef INIT_SPINBOX
}

void MainWindow::setROILimits()
{
    int w = img.width(), h = img.height();
    RoiManager *mgr = RoiManager::GetInstance();
    auto sidx = mgr->getSelectedIndices();
    QString roinameval, classnameval;

    if (sidx.size() == 1)
    {
        QRectF sr = mgr->getROI(sidx[0])->rect();
        roinameval = QString::fromStdString(mgr->getROIName(sidx[0]));
        classnameval = QString::fromStdString(mgr->getClassName(sidx[0]));

        if (roiname->text() != roinameval)
            roiname->setText(roinameval);
        if (classname->text() != classnameval)
            classname->setText(classnameval);

        if (spx->maximum() != (w - sr.width()))
            spx->setMaximum(w - sr.width());
        if (spy->maximum() != (h - sr.height()))
            spy->setMaximum(h - sr.height());

        if (spw->maximum() != (w - sr.left()))
            spw->setMaximum(w - sr.left());
        if (sph->maximum() != (h - sr.top()))
            sph->setMaximum(h - sr.top());
    }
    else
    {
        int mr = 0, mb = 0, ml = 0, mt = 0;

        for (int i = 0; i < sidx.size(); i++)
        {
            QRectF sr = mgr->getROI(sidx[i])->rect();

            mr = MAX(mr, sr.width());
            mb = MAX(mb, sr.height());
            ml = MAX(ml, sr.left());
            mt = MAX(mt, sr.top());
        }

        if (spx->maximum() != (w - mr))
            spx->setMaximum(w - mr);
        if (spy->maximum() != (h - mb))
            spy->setMaximum(h - mb);

        if (spw->maximum() != (w - ml))
            spw->setMaximum(w - ml);
        if (sph->maximum() != (h - mt))
            sph->setMaximum(h - mt);
    }
}

void MainWindow::updateROIDimensions()
{
    static bool recursive = false;

    // Prevent recursive execution
    if (recursive)
        return;

    recursive = true;
    updatingROI = true;
    RoiManager *mgr = RoiManager::GetInstance();
    int minx = INT_MAX, maxx = 0, miny = INT_MAX, maxy = 0;
    int w = img.width(), h = img.height();
    vector<QString> roilblnames;
    vector<int> selectedidx;
    
    if (!uilistselecting)
    {
        updatingroilist = true;

        for (int i = 0; i < mgr->getROICount(); i++)
        {
            roilblnames.push_back(QString::fromStdString(mgr->getROIName(i)));
        }
        selectedidx = mgr->getSelectedIndices();
        for (int i = roilist->count() - 1; i >= 0; i--)
            delete roilist->takeItem(i);
        for (int i = 0; i < mgr->getROICount(); i++)
        {
            roilist->addItem(roilblnames[i]);

            if (std::find(selectedidx.begin(), selectedidx.end(), i) != selectedidx.end())
                roilist->item(i)->setSelected(true);
        }

        updatingroilist = false;
    }

    if (spw->maximum() != w)
        spw->setMaximum(w);
    if (sph->maximum() != h)
        sph->setMaximum(h);

    if (mgr->getSelectedROICount() == 0)
    {
        roiname->setText("");
        classname->setText("");
        spx->setValue(0);
        spy->setValue(0);
        spw->setValue(0);
        sph->setValue(0);

        roiname->setReadOnly(true);
        classname->setReadOnly(true);
        spx->setReadOnly(true);
        spy->setReadOnly(true);
        spw->setReadOnly(true);
        sph->setReadOnly(true);

        recursive = false;
        updatingROI = false;
        return;
    }
    else if (mgr->getSelectedROICount() == 1)
    {
        int sidx = mgr->getSelectedIndices()[0];
        QRectF sr = mgr->getROI(sidx)->rect();

        roiname->setReadOnly(false);
        classname->setReadOnly(false);
        spx->setReadOnly(false);
        spy->setReadOnly(false);
        spw->setReadOnly(false);
        sph->setReadOnly(false);

        setROILimits();

        if (spx->value() != sr.left())
            spx->setValue(sr.left());
        if (spy->value() != sr.top())
            spy->setValue(sr.top());
        if (spw->value() != sr.width())
            spw->setValue(sr.width());
        if (sph->value() != sr.height())
            sph->setValue(sr.height());

        recursive = false;
        updatingROI = false;
        return;
    }
    else
    {
        vector<int> si = mgr->getSelectedIndices();
        
        bool diff = false;

        QString nm = QString::fromStdString(mgr->getClassName(si[0]));

        QRectF re, ri;
        bool rdiff[4] = { false, false, false,false };

        for (int i = 1; i < si.size(); i++)
        {
            diff = diff || (mgr->getClassName(si[i]) != nm.toStdString());
            if (diff)
                break;
        }

        if (diff)
            classname->setText("");
        else
        {
            if(classname->text() != nm)
                classname->setText(nm);
        }

        nm = QString::fromStdString(mgr->getROIName(si[0]));

        for (int i = 1; i < si.size(); i++)
        {
            diff = diff || (mgr->getROIName(si[i]) != nm.toStdString());
            if (diff)
                break;
        }

        if (diff)
            roiname->setText("");
        else
        {
            if (roiname->text() != nm)
                roiname->setText(nm);
        }

        re = mgr->getROI(si[0])->rect();
        setROILimits();

        for (int i = 1; i < si.size(); i++)
        {
            ri = mgr->getROI(si[1])->rect();

            rdiff[0] = rdiff[0] || (re.left() != ri.left());
            rdiff[1] = rdiff[1] || (re.top() != ri.top());
            rdiff[2] = rdiff[2] || (re.width() != ri.width());
            rdiff[3] = rdiff[3] || (re.height() != ri.height());
        }

        if (rdiff[0])
            spx->setValue(0);
        else if (spx->value() != re.left())
            spx->setValue(re.left());
        
        if (rdiff[1])
            spy->setValue(0);
        else if (spy->value() != re.top())
            spy->setValue(re.top());
        
        if (rdiff[2])
            spw->setValue(0);
        else if (spw->value() != re.width())
            spw->setValue(re.width());
        
        if (rdiff[3])
            sph->setValue(0);
        else if (sph->value() != re.height())
            sph->setValue(re.height());
        
        recursive = false;
        updatingROI = false;
        return;
    }
}

void MainWindow::processImage()
{
    if (input.empty())
    {
        qCritical() << "Image not loaded.";
        return;
    }

    IPlugin *pl = nullptr;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();

        if (plugins.size() == 0)
        {
            qCritical() << "No plugins loaded.";
            return;
        }

        if (!(pnamelist->currentIndex() >= 0 && pnamelist->currentIndex() < plugins.size()))
            return;

        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;

    pl->setImage(input, imgbasename);
    ProgressReporter *pr = nullptr;

    RoiManager *mgr = RoiManager::GetInstance();
    auto roiitems = mgr->getROIs();
    vector<QRectF> roirectitems;

    for (int ii = 0; ii < roiitems.size(); ii++)
    {
        roirectitems.push_back(roiitems[ii]->rect());
    }

    int roicount = mgr->getROICount();

    if (pl->getProgressSteps() > 0)
    {
//#ifndef _DEBUG
        pr = new ProgressReporter(pl);
        pr->start(); // Start the processing in the thread.
        pr->open();  // Show the user the progress bar
        pr->exec();  // Perform event handling
//#else
        //pl->execute();
//#endif
    }
    else
        pl->execute();

    if (pr != nullptr)
    {
        if (pr->wasCanceled())
        {
            pr->deleteLater();
            pr = nullptr;
            return;
        }
    }

    //updateProgress("Displaying output ... ");
    output = pl->getOutputImages();
    displayidx = pl->getDisplayIndex();
    setImage(output[displayidx - 1]);

    QChart *resultchart = pl->getChart();

    if (resultchart != nullptr)
        showChartWidget(resultchart);
    
    //RoiManager *mgr = RoiManager::GetInstance();
    //int roicount = mgr->getROICount();
    bool isColNamesEqual = true;

    auto colnames = pl->getCSVColumnNames();

    if (colnamelist.size() != colnames.size())
        isColNamesEqual = false;
    else
    {
        for (int i = 0; i < colnames.size(); i++)
        {
            if (colnamelist[i] != colnames[i])
            {
                isColNamesEqual = false;
                break;
            }
        }
    }

    if (pl->getOutputType() == OutputType::ImageAndValues ||
        pl->getOutputType() == OutputType::ImagesAndValues ||
        pl->getOutputType() == OutputType::Values)
    {
        if (roicount == 0)
        {
            vector<double> feats = pl->getFeatures();

            //if (featureitemmodel->columnCount() != (feats.size() + 2))
            if (!isColNamesEqual)
                reinitfeatureview();

            featureviewsync.push_back(feats);
            roisync.push_back("Full");
            //filenamesync.push_back(QString::fromStdString(experimental::filesystem::path(inputfilename.toStdString()).filename().string()));
            filenamesync.push_back(imgbasename);

            int row = featureitemmodel->rowCount();
            //QStandardItem *item = new QStandardItem(QString::fromStdString(experimental::filesystem::path(inputfilename.toStdString()).filename().string()));
            QStandardItem *item = new QStandardItem(imgbasename);
            featureitemmodel->setItem(row, 0, item);
            item = new QStandardItem(QString::fromStdString("Full"));
            featureitemmodel->setItem(row, 1, item);

            for (int column = 2; column < (feats.size() + 2); ++column)
            {
                QStandardItem *item;

                if (isnan(feats[column - 2]))
                    item = new QStandardItem(QString::fromStdString("NA"));
                else
                    item = new QStandardItem(QString("%0").arg(feats[column - 2], 0, 'f', 3));
                featureitemmodel->setItem(row, column, item);
            }
        }
        else
        {
            for (int i = 0; i < roicount; i++)
            {
                vector<double> feats = pl->getFeatures(i);

                if (feats.size() == 0)
                    continue;

                //if (featureitemmodel->columnCount() != (feats.size() + 2))
                if (!isColNamesEqual)
                    reinitfeatureview();

                featureviewsync.push_back(feats);

                QString roinameval = QString::fromStdString(mgr->getROIName(i));
                roisync.push_back(roinameval);
                filenamesync.push_back(QString::fromStdString(experimental::filesystem::path(inputfilename.toStdString()).filename().string()));

                int row = featureitemmodel->rowCount();
                QStandardItem *item = new QStandardItem(QString::fromStdString(experimental::filesystem::path(inputfilename.toStdString()).filename().string()));
                featureitemmodel->setItem(row, 0, item);
                item = new QStandardItem(roinameval);
                featureitemmodel->setItem(row, 1, item);

                for (int column = 2; column < (feats.size() + 2); ++column)
                {
                    QStandardItem *item;

                    if (isnan(feats[column - 2]))
                        item = new QStandardItem(QString::fromStdString("NA"));
                    else
                        item = new QStandardItem(QString("%0").arg(feats[column - 2], 0, 'f', 3));
                    featureitemmodel->setItem(row, column, item);
                }
            }
        }
        
        featureWidget->raise();
    }

    if (pr != nullptr)
    {
        pr->deleteLater();
        pr = nullptr;
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize && obj == actiondocker)
    {
        //vl->update();
        actiondocker->update();
        return QWidget::eventFilter(obj, event);
    }
    else if (event->type() == QEvent::KeyPress && obj == logbox)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Space)
        {
            event->ignore();
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (event->type() == QEvent::KeyPress && obj == featureview)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->matches(QKeySequence::Copy))
            copyItems();

        return true;
    }
    else
        return false;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && (!input.empty()) && (!output.empty()))
    {
        IPlugin *pl = nullptr;

        if (!mainplugin)
        {
            PluginManager *p = PluginManager::GetInstance();
            auto plugins = p->GetPlugins();
            pl = plugins[pnamelist->currentIndex()];
        }
        else
        {
            pl = mainplugin;
        }

        int ncount = 0;

        while (true)
        {
            displayidx++;

            if (displayidx > output.size())
                displayidx = 0;

            if (displayidx == 0)
                ncount++;

            if (ncount > 1)
                break;
            if (displayidx == 0 && (!input.empty()))
                break;
            else if (!output[displayidx - 1].empty())
                break;
        }

        if (ncount > 1)
            return;

        if (displayidx == 0)
        {
            if (invertColors->isChecked())
                setImage(Scalar::all(255) - input);
            else
                setImage(input);
        }
        else
            setImage(output[displayidx - 1]);
        pl->setDisplayIndex(displayidx);
    }
    else
    {
        QMainWindow::keyPressEvent(event);
        return;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    /*if (event->mimeData()->hasFormat("text/uri-list"))
    {
        const QMimeData* mimeData = event->mimeData();

        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        if (urlList.size() == 1)
        {
            QString filename = urlList.at(0).toLocalFile();
            
            if (filename.endsWith(".png", Qt::CaseInsensitive) ||
                filename.endsWith(".jpg", Qt::CaseInsensitive) ||
                filename.endsWith(".jpeg", Qt::CaseInsensitive) ||
                filename.endsWith(".bmp", Qt::CaseInsensitive) ||
                filename.endsWith(".tif", Qt::CaseInsensitive) ||
                filename.endsWith(".tiff", Qt::CaseInsensitive))
            {*/
                event->acceptProposedAction();
                /*       }
        }
    }*/
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        open(event->mimeData()->urls().at(0).toLocalFile());
        event->acceptProposedAction();
    }
}

void MainWindow::showChartWidget(QChart *chart)
{
    static int displaychart = 1;

    QChartView *cv = new QChartView(chart);
    cv->setContentsMargins(0, 0, 0, 0);
    chartWidget->setWidget(cv);

    if (displaychart == 1 || (displaychart == 0 && chartWidget->isVisible()))
    {
        chartWidget->show();
        chartWidget->update();
    }

    displaychart = 0;
}

//void GraphicsView::keyPressEvent(QKeyEvent *event)
//{
//    QWidget::keyPressEvent(event);
//}

void MainWindow::addNewDockWidget(QDockWidget *w)
{
    tabifyDockWidget(logdocker, w);
}

MainWindow::MainWindow()
{
    RoiManager *mgr = RoiManager::GetInstance();
    mgr->setMessageHandler(logger);
    //setMinimumSize(800,600);
    chartWidget = new QDockWidget("Histogram", this);
    chartWidget->setContentsMargins(0, 0, 0, 0);
    this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, chartWidget);
    chartWidget->setVisible(false);

    // To first set the logging sub-system.
    logdocker = new QDockWidget("Log", this);
    logdocker->setObjectName(logdocker->windowTitle());
    logbox = new QTextEdit(logdocker);
    logdocker->setWidget(logbox);
    logbox->setReadOnly(true);
    logbox->installEventFilter(this);
    connect(logbox, &QTextEdit::textChanged, this, &MainWindow::logUpdated);

    logdocker->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, logdocker);
    qInstallMessageHandler(logger);

    actiondocker = new QDockWidget("", this);
    actiondocker->setAllowedAreas(Qt::LeftDockWidgetArea);
    actiondocker->setMinimumWidth(100);
    actiondocker->installEventFilter(this);
    //connect(actiondocker, &QDockWidget::
    
    roipropdocker = new QDockWidget("ROI Manager", this);
    roipropdocker->setAllowedAreas(Qt::RightDockWidgetArea);
    roipropdocker->setMinimumWidth(200);
    roipropdocker->installEventFilter(this);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, roipropdocker);

    featureitemmodel = new QStandardItemModel();
    featureWidget = new QDockWidget("Features", this);
    featureview = new QTableView();
    featureview->setModel(featureitemmodel);
    featureview->installEventFilter(this);
    featureWidget->setWidget(featureview);
    addNewDockWidget(featureWidget);
    //featureview->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    featureview->setHorizontalHeader(new HeaderViewWithWordWrap(featureitemmodel, Qt::Horizontal, this));
    featureview->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    featureview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    featureview->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    featureview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(featureview, &QTableView::customContextMenuRequested, this, &MainWindow::viewcontextmenu);
    connect(featureitemmodel, &QStandardItemModel::rowsInserted, featureview, &QTableView::scrollToBottom);

    fswatcher = new QFileSystemWatcher();
    connect(fswatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::watcherDirectoryChanged);
    
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

    createmenus();
    createtoolbars();

    QWidget *wid = new QWidget(this);
    QVBoxLayout *vl2 = new QVBoxLayout();
    vl2->setContentsMargins(0, 0, 0, 0);
    
    view = new GraphicsView();
    connect(view, &GraphicsView::showPixelInfo, this, &MainWindow::displayPixelInfo);
    connect(view, &GraphicsView::openFile, [&](QString filename)
    {
        //this, static_cast<void (MainWindow::*)(QString)>(&MainWindow::open)
        open(filename);
    });

    view->setAcceptDrops(true);
    setROIUI();

    //vl2->addWidget(tbox);
    //vl2->addWidget(area);
    vl2->addWidget(view);
    wid->setLayout(vl2);

    setCentralWidget(wid);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setAcceptDrops(true);
    setWindowFlags(windowFlags() | Qt::WindowType::WindowContextHelpButtonHint);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);

    this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, actiondocker);

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool isvalidcharacter(int keyval)
{
    char key = char(keyval);
    return (isprint(keyval) &&
        key != '<' &&
        key != '>' &&
        key != ':' &&
        key != '/' &&
        key != '\\' &&
        key != '|' &&
        key != '?' &&
        key != '*' &&
        key != '"');
}

//void MainWindow::formatchanged()
//{
//    QAction *action = qobject_cast<QAction *>(sender());
//    QString desc = action->text();
//
//    for (int i = 0; i < formatdesc.size(); i++)
//    {
//        if (formatdesc[i] == desc)
//        {
//            formatidx = i;
//            break;
//        }
//    }
//}

void MainWindow::createmenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QMenu *openMenu = fileMenu->addMenu(tr("Open"));
    QAction *openAct = openMenu->addAction(tr("&Image"), this, static_cast<void (MainWindow::*)()>(&MainWindow::open));

    QAction *saveAct = fileMenu->addAction(tr("&Save"), this, &MainWindow::save);
    
    QMenu *saveasMenu = fileMenu->addMenu(tr("Save As"));
    QAction *saveasAct = saveasMenu->addAction(tr("Displayed image"), [&]()
    {
        saveas(SaveAsMode::DisplayedImage);
    });
    
    saveasAct = saveasMenu->addAction(tr("Annotation"), [&]()
    {
        saveas(SaveAsMode::Annotation);
    });

    saveasAct = saveasMenu->addAction(tr("Features"), [&]()
    {
        saveas(SaveAsMode::Features);
    });

    saveasAct = saveasMenu->addAction(tr("Settings"), [&]()
    {
        saveas(SaveAsMode::Settings);
    });

    fileMenu->addSeparator();
    QAction *loadAnnotationAct = openMenu->addAction(tr("&Load Annotation"), this, &MainWindow::loadAnnotation);

    QAction *loadSettingsAct = openMenu->addAction(tr("&Load Settings"), this, [&]()
    {
        IPlugin *pl;

        if (!mainplugin)
        {
            PluginManager *p = PluginManager::GetInstance();
            auto plugins = p->GetPlugins();
            pl = plugins[pnamelist->currentIndex()];
        }
        else
            pl = mainplugin;

        QString fileName = QFileDialog::getOpenFileName(this, tr("Load Annotation"),
            QString(), tr("CSV file (*.csv)"));
        if (!fileName.isEmpty())
        {
            pl->loadMetadata(fileName);
        }
    });
    //QAction *saveAnnotationAct = fileMenu->addAction(tr("&Save Annotation"), this, &MainWindow::saveAnnotation);
    //fileMenu->addSeparator();
    QAction *batchanalysisAct = fileMenu->addAction(tr("Batch Analysis"), this, &MainWindow::batchAnalysis);
    fileMenu->addSeparator();

    /*QMenu *saveformat = fileMenu->addMenu("Save File Format");
    QActionGroup *formatactiongroup = new QActionGroup(this);

    for (int i = 0; i < formats.size(); i++)
    {
        QAction *action = new QAction(formatdesc[i], this);
        action->setCheckable(true);
        formatactiongroup->addAction(action);
        if (formats[i] == "PNG")
            action->setChecked(true);
        connect(action, &QAction::triggered, this, &MainWindow::formatchanged);
    }

    saveformat->addActions(formatactiongroup->actions());*/

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QMenu *roiMenu = editMenu->addMenu("ROI");
    QAction *roiclearAct = new QAction("Clear all ROIs", this);
    connect(roiclearAct, &QAction::triggered, [&]()
    {
        RoiManager *mgr = RoiManager::GetInstance();
        auto items = mgr->getROIs();
        for (int idx = 0; idx < items.size(); idx++)
            view->scene()->removeItem(items[idx]);
        mgr->deleteAll();
    });
    roiMenu->addAction(roiclearAct);
    fileMenu->addSeparator();
    QMenu *pixelstat = editMenu->addMenu("Pixel statistics");

    QActionGroup *formatactiongroup = new QActionGroup(this);

    for (int i = 0; i < pixelformats.size(); i++)
    {
        QAction *action = new QAction(pixelformats[i], this);
        action->setCheckable(true);
        formatactiongroup->addAction(action);
        if (pixelformats[i] == "RGB")
            action->setChecked(true);
        connect(action, &QAction::triggered, this, &MainWindow::pixelformatchanged);
    }

    pixelstat->addActions(formatactiongroup->actions());

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(actiondocker->toggleViewAction());
    viewMenu->addAction(logdocker->toggleViewAction());
    viewMenu->addAction(roipropdocker->toggleViewAction());
    viewMenu->addAction(featureWidget->toggleViewAction());
    viewMenu->addAction(chartWidget->toggleViewAction());

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}

void MainWindow::pixelformatchanged()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString desc = action->text();
    pfmt = desc;
}

void MainWindow::batchAnalysis()
{
    mainplugin->setBatchMode(true);
    BatchProcessor *dialog = new BatchProcessor(mainplugin, inputfileloc);
    dialog->setWindowIcon(appicon);
    dialog->setWindowFlags(dialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog->setWindowTitle("Batch Process");
    dialog->open();
    dialog->exec();

    dialog->deleteLater();
    mainplugin->setBatchMode(false);
}

void MainWindow::createtoolbars()
{
    // Tool bar for setting mouse mode.
    mousemodetb = QMainWindow::addToolBar("mousemode");
    QActionGroup *ag = new QActionGroup(this);
    QAction *act = new QAction(QIcon(":/icons/panhand"), tr("Image panning mode."), this);
    act->setStatusTip("Image panning mode.");
    act->setCheckable(true);
    act->setActionGroup(ag);
    act->setChecked(true);
    connect(act, &QAction::triggered, [&]()
    {
        /*QAction* action = qobject_cast<QAction*>(sender());

        if (action != nullptr && action->isChecked())
        {*/
            view->setMouseMode(MouseMode::ImagePanning);
            view->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        //}
    });
    mousemodetb->addAction(act);
        
    act = new QAction(QIcon(":/icons/eyedropper"), tr("Pixel value mode."), this);
    act->setStatusTip("Pixel value mode.");
    act->setCheckable(true);
    act->setActionGroup(ag);
    connect(act, &QAction::triggered, [&]()
    {
        /*QAction* action = qobject_cast<QAction*>(sender());

        if (action != nullptr && action->isChecked())
        {*/
            view->setMouseMode(MouseMode::PixelValue);
            view->setDragMode(QGraphicsView::DragMode::NoDrag);

            QCursor sh(Qt::CursorShape::ArrowCursor);
            view->viewport()->setCursor(sh);
        //}
    });
    mousemodetb->addAction(act);

    roitoolbaract = new QAction(QIcon(":/icons/regionofinterest"), tr("Region of interest mode."), this);
    roitoolbaract->setStatusTip("Region of interest mode.");
    roitoolbaract->setCheckable(true);
    roitoolbaract->setActionGroup(ag);
    connect(roitoolbaract, &QAction::triggered, [&]()
    {
        /*QAction* action = qobject_cast<QAction*>(sender());

        if (action != nullptr && action->isChecked())
        {*/
            view->setMouseMode(MouseMode::RegionOfInterest);
            view->setDragMode(QGraphicsView::DragMode::NoDrag);

            QCursor sh(Qt::CursorShape::CrossCursor);
            view->viewport()->setCursor(sh);
        //}
    });
    mousemodetb->addAction(roitoolbaract);
    mousemodetb->setEnabled(true);

    // Tool bar for zooming.
    zoomtb = QMainWindow::addToolBar("zoom");
    act = new QAction(QIcon(":/icons/mag-glass-zoomout-box"), tr("Zoom out."), this);
    act->setShortcut(QKeySequence::ZoomOut);
    act->setStatusTip(tr("Zoom out."));
    connect(act, &QAction::triggered, this, &MainWindow::zoomout);

    zoomtb->addAction(act);

    act = new QAction(QIcon(":/icons/mag-glass-zoomin-box"), tr("Zoom in."), this);
    act->setShortcut(QKeySequence::ZoomIn);
    act->setStatusTip(tr("Zoom in."));
    connect(act, &QAction::triggered, this, &MainWindow::zoomin);

    zoomtb->addAction(act);

    act = new QAction(QIcon(":/icons/fittowindow"), tr("Zoom to fit."), this);
    act->setStatusTip(tr("Zoom to fit."));
    connect(act, &QAction::triggered, this, &MainWindow::zoomfit);

    zoomtb->addAction(act);
    zoomtb->setDisabled(true);

    // Tool bar for pixel statistics.
    pixelstattb = QMainWindow::addToolBar("pixelstats");
    act = new QAction(QIcon(":/icons/pixelmeasureswholeimage"), tr("Image color statistics."), this);
    //act->setShortcut(QKeySequence::ZoomOut);
    act->setStatusTip(tr("Print image color statistics."));
    connect(act, &QAction::triggered, [&]()
    {
        displayImageStats({ QRectF(0, 0, input.cols, input.rows) });
    });

    pixelstattb->addAction(act);

    act = new QAction(QIcon(":/icons/pixelmeasuresROI"), tr("Region-of-interest color statistics."), this);
    //act->setShortcut(QKeySequence::ZoomIn);
    act->setStatusTip(tr("Print region-of-interest color statistics."));
    connect(act, &QAction::triggered, [&]()
    {
        RoiManager *mgr = RoiManager::GetInstance();
        auto rs = mgr->getROIs();
        vector<QRectF> rects;

        for (auto &r : rs)
            rects.push_back(r->rect());
        
        displayImageStats(rects);
    });

    pixelstattb->addAction(act);
    pixelstattb->setEnabled(false);

    // Toolbar for browsing images
    browseimagetb = QMainWindow::addToolBar("browseimages");
    act = new QAction(QIcon(":/icons/left-arrow"), tr("Load previous"), this);
    act->setShortcut(QKeySequence::MoveToPreviousChar);
    act->setStatusTip(tr("Load previous image."));
    connect(act, &QAction::triggered, [&]()
    {
        loadNextImage(false);
    });

    browseimagetb->addAction(act);

    act = new QAction(QIcon(":/icons/right-arrow"), tr("Load next"), this);
    act->setShortcut(QKeySequence::MoveToNextChar);
    act->setStatusTip(tr("Load next image."));
    connect(act, &QAction::triggered, [&]()
    {
        loadNextImage(true);
    });

    browseimagetb->addAction(act);
    browseimagetb->setEnabled(false);

    executetb = QMainWindow::addToolBar("Execute");
    act = new QAction(QIcon(":/icons/startfeatureanalysis"), tr("Run analysis"), this);
    act->setStatusTip(tr("Start analysis."));
    connect(act, &QAction::triggered, [&]()
    {
        processImage();
    });
    executetb->addAction(act);
    executetb->setEnabled(false);

    colinvtb = QMainWindow::addToolBar("Invert colors");
    invertColors = new QAction(QIcon(":/icons/invert_image"), tr("Invert colors."), this);
    invertColors->setStatusTip(tr("Invert colors."));
    invertColors->setCheckable(true);
    connect(invertColors, &QAction::triggered, [&]()
    {
        if (img.width() == 0)
            return;

        if (displayidx == 0)
        {
            if (invertColors->isChecked())
                setImage(Scalar::all(255) - input);
            else
                setImage(input);
        }
    });

    colinvtb->addAction(invertColors);
    colinvtb->setEnabled(false);

    rottb = QMainWindow::addToolBar("rotate");
    lbrotation = new QLabel("Image clockwise rotation: ");
    cbrotation = new QComboBox();
    QChar degreeChar(0260);
    cbrotation->addItem(QString("0%1").arg(degreeChar));
    cbrotation->addItem(QString("90%1").arg(degreeChar));
    cbrotation->addItem(QString("180%1").arg(degreeChar));
    cbrotation->addItem(QString("270%1").arg(degreeChar));
    cbrotation->setCurrentIndex(0);

    connect(cbrotation, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int k)
    {
        //setRotation(0);
        setRotation(k * 90);
    });

    rottb->addWidget(lbrotation);
    rottb->addWidget(cbrotation);

    rottb->setDisabled(true);
    //hostWindow->addToolBar(tbar);
}

void MainWindow::setImage(Mat m)
{
    if (m.channels() == 1)
    {
        img = QImage(m.data,
            m.cols, m.rows, static_cast<int>(m.step),
            QImage::Format_Grayscale8);
    }
    else
    {
        
        img = QImage(m.data,
            m.cols, m.rows, static_cast<int>(m.step),
            QImage::Format_RGB888);
    }

    RoiManager *mgr = RoiManager::GetInstance();
    mgr->clearSelection();

    view->setPixmap(img);
    emit imageChanged();

    if (!zoomtb->isEnabled())
    {
        zoomtb->setEnabled(true);
        rottb->setEnabled(true);
        mousemodetb->setEnabled(true);
        pixelstattb->setEnabled(true);
        browseimagetb->setEnabled(true);
        executetb->setEnabled(true);
        colinvtb->setEnabled(true);
        plugintb->setEnabled(true);

        view->setEnabledTransformations(true);
    }

    //view->SetScale(1.0);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", 
        QString(), tr("Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff)"));
    open(fileName);
    /*if (!fileName.isEmpty())
    {
        Mat mi = imread(fileName.toStdString(), IMREAD_ANYCOLOR);
        input = mi.clone();

        if (input.empty())
            qCritical() << "Error loading image.";
        else
        {
            if (input.channels() == 3)
                cvtColor(input, input, COLOR_BGR2RGB);

            setImage(input);
            zoomfit();
        }
    }*/
}

void MainWindow::loadNextImage(bool front)
{
    if (currimagelistidx == -1)
        return;

    if (front)
    {
        currimagelistidx++;

        if (imagelist.size() == currimagelistidx)
            currimagelistidx = 0;
    }
    else
    {
        currimagelistidx--;

        if (currimagelistidx < 0)
            currimagelistidx = imagelist.size() - 1;
    }

    open(inputfileloc + "/" + imagelist[currimagelistidx], false);
}

void MainWindow::populateFileList()
{
    auto loc = QDir(inputfileloc);
    
    QStringList filt;
    filt << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.tif" << "*.tiff";
    imagelist = loc.entryList(filt, QDir::Files, QDir::Name);

    currimagelistidx = -1;
    
    for (int i = 0; i < imagelist.size(); i++)
    {
        if (imagelist[i] == imgbasename)
        {
            currimagelistidx = i;
            break;
        }
    }
}

void MainWindow::watcherDirectoryChanged(const QString & path)
{
    populateFileList();
}

void MainWindow::open(QString filename, bool refreshlist)
{
    IPlugin *pl;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;

    if (!filename.isEmpty())
    {
        Mat mi = imread(filename, IMREAD_ANYCOLOR);
        input = mi.clone();

        if (input.empty())
            qCritical() << "Error loading image.";
        else
        {
            if (input.channels() == 3)
                cvtColor(input, input, COLOR_BGR2RGB);

            inputfilename = filename;
            displayidx = 0;
            QFileInfo finfo(inputfilename);

            pl->setImage(input, finfo.fileName());

            if (invertColors->isChecked())
                setImage(Scalar::all(255) - input);
            else
                setImage(input);
            
            zoomfit();
            output.clear();

            qInfo() << "Image " << inputfilename << " loaded.";

            if (refreshlist)
            {
                if (inputfileloc.length() > 0)
                    fswatcher->removePath(inputfileloc);

                inputfileloc = finfo.path();
                imgbasename = finfo.fileName();
                fswatcher->addPath(inputfileloc);
                populateFileList();
            }
            else
                imgbasename = finfo.fileName();
        }
    }
}

void MainWindow::save()
{
    if (savefile.length() == 0)
        saveas(currsavemode);
    else
        saveas(currsavemode, savefile);
}

void MainWindow::saveas(SaveAsMode savmode, QString fileName)
{
    QString ofileName = fileName;
    int dispidx = -1;
    RoiManager *mgr = RoiManager::GetInstance();
    IPlugin *pl;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
        dispidx = plugins[pnamelist->currentIndex()]->getDisplayIndex();
    }
    else
    {
        pl = mainplugin;
        dispidx = mainplugin->getDisplayIndex();
    }

    QFileInfo finfo(inputfilename);

    switch (savmode)
    {
    case SaveAsMode::DisplayedImage:
        if ((dispidx == 0 && input.empty()) || (dispidx > 0 && output[dispidx - 1].empty()))
        {
            qCritical() << "Cannot save empty image.";
            return;
        }

        if (ofileName.length() == 0)
        {
            ofileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                inputfileloc + "/" + finfo.completeBaseName() + "_output.png", tr("Images (*.png *.jpg *.bmp *.tiff)"));
        }
        if (!ofileName.isEmpty())
        {
            savefile = ofileName;
            currsavemode = savmode;
            Mat wout;
            
            if (dispidx == 0)
            {
                input.copyTo(wout);
                if (invertColors->isChecked())
                    wout = Scalar::all(255) - wout;
            }
            else if (output[dispidx - 1].channels() == 3)
                cvtColor(output[dispidx - 1], wout, COLOR_BGR2RGB);
            else
                output[dispidx - 1].copyTo(wout);

            RotateFlags flag = ROTATE_90_CLOCKWISE;

            switch (view->getRotation())
            {
            case 90:
                flag = ROTATE_90_CLOCKWISE;
                break;
            case 180:
                flag = ROTATE_180;
                break;
            case 270:
                flag = ROTATE_90_COUNTERCLOCKWISE;
                break;
            }

            if (view->getRotation() != 0)
                rotate(wout, wout, flag);
            
            imwrite(savefile, wout);
            qInfo() << "Image saved as " << ofileName;
        }

        break;
    case SaveAsMode::Annotation:
        if (mgr->getROICount() == 0)
        {
            qCritical() << "No regions of interest found.";
            return;
        }

        if (ofileName.length() == 0)
        {
            ofileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                inputfileloc + "/" + finfo.completeBaseName() + "_annotation.txt", tr("Text file (*.txt)"));
        }

        if (!ofileName.isEmpty())
        {
            savefile = ofileName;
            currsavemode = savmode;
            mgr->saveAnnotation(ofileName);
            qInfo() << "Annotation saved as " << ofileName;
        }
        
        break;
    case SaveAsMode::Features:
        if (featureitemmodel->rowCount() == 0)
        {
            qCritical() << "No features found.";
            return;
        }

        if (ofileName.length() == 0)
            ofileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                inputfileloc + "/" + "features.csv", tr("Text file (*.txt *.csv)"));
        if (!ofileName.isEmpty())
        {
            QFileInfo ofinfo(ofileName);

            QString outputfileloc = ofinfo.path();
            QString oimgbasename = ofinfo.fileName();
            
            if (outputfileloc.length() > 0 && (outputfileloc[outputfileloc.length() - 1] != '/' || outputfileloc[outputfileloc.length() - 1] != '\\'))
                outputfileloc += "/";
            
            //QString textData;
            int rows = featureitemmodel->rowCount();
            int columns = featureitemmodel->columnCount();
            //auto colnames = pl->getCSVColumnNames();

            ////textData += colnames[0];
            //f << colnames[0].toStdString();
            //for (int j = 1; j < colnames.size(); j++)
            //{
            //    //textData += ", ";
            //    //textData += colnames[j];
            //    f << ", " << colnames[j].toStdString();
            //}
            ////textData += "\n";
            //f << endl;
            pl->writeHeader(outputfileloc, oimgbasename);

            ofstream f;
            f.open(ofileName.toStdString(), ios::out | ios::app);

            for (int i = 0; i < rows; i++)
            {
                //textData += featureitemmodel->data(featureitemmodel->index(i, 0)).toString();
                //textData += (filenamesync[i] + ", " + roisync[i]);
                f << filenamesync[i].toStdString() << ", " << roisync[i].toStdString();

                for (int j = 2; j < columns; j++)
                {
                    //textData += ", ";
                    ////textData += featureitemmodel->data(featureitemmodel->index(i, j)).toString();
                    //textData += featureviewsync[i][j - 2];
                    if (!(isnan(featureviewsync[i][j - 2]) || isinf(featureviewsync[i][j - 2])))
                        f << ", " << std::setprecision(ceil(log10(abs(featureviewsync[i][j - 2]))) + 6) << featureviewsync[i][j - 2];
                    else
                        f << "," << "NA";
                }
                //textData += "\n";
                f << endl;
            }

            f.close();
            qInfo() << "Features saved as " << ofileName;
            savefile = ofileName;
            currsavemode = savmode;

            /*QFile txtFile(ofileName);
            if (txtFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                QTextStream out(&txtFile);
                out << textData;
                txtFile.close();
                qInfo() << "Features saved as " << ofileName;
            }*/
        }

        break;
    case SaveAsMode::Settings:
        if (ofileName.length() == 0)
            ofileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                inputfileloc + "/" + "metadata.csv", tr("CSV file (*.csv)"));
        if (!ofileName.isEmpty())
        {
            // ofstream f;
            // f.open(ofileName.toStdString(), ios::out);
            pl->saveMetadata("", "", ofileName);
            savefile = ofileName;
            currsavemode = savmode;
            //f.close();
            qInfo() << "Settings saved as " << ofileName;
        }
        break;
    }

    /*if (!fileName.isEmpty())
    {
        savefile = fileName.toStdString();
        currsavemode = savmode;
    }*/
}

void MainWindow::about()
{
    QDialog *about = new QDialog(this);
    about->setWindowIcon(appicon);
    
    if (mainplugin == nullptr)
        about->setWindowTitle(tr("About Image Segmenter"));
    else
        about->setWindowTitle(QString::fromStdString("About " + mainplugin->getName()));

    QVBoxLayout *vl = new QVBoxLayout();
    QHBoxLayout *layout = new QHBoxLayout();

    QLabel *lb = new QLabel();
    lb->setPixmap(appimage);
    lb->setMaximumWidth(200);
    lb->setMaximumHeight(200);
    lb->setScaledContents(true);

    layout->addWidget(lb, 0);

    /*lb = new QLabel(tr("This program segments images using various segmentation algorithms \n"
        "available in computer vision literature. This program can be extended using plugins, \n"
        "to incorporate new technologies and algorithms."));*/
    //lb = new QLabel(tr(abttxt.c_str()));
    //layout->addWidget(lb, 0);
    QTextBrowser  *tlb = new QTextBrowser();

    if (mainplugin == nullptr)
    {
        lb = new QLabel(tr("This program segments images using various segmentation algorithms \n"
        "available in computer vision literature. This program can be extended using plugins, \n"
        "to incorporate new technologies and algorithms."));
        lb = new QLabel(tr(abttxt.c_str()));
        layout->addWidget(lb, 0);
    }
    else
    {
        tlb->setHtml(QString::fromStdString(abttxt));
        tlb->setReadOnly(true);
        tlb->setOpenExternalLinks(true);
        layout->addWidget(tlb, 0);
    }

    vl->addLayout(layout);

    QPushButton *btn = new QPushButton("Ok");
    connect(btn, &QPushButton::clicked, about, &QDialog::close);

    vl->addWidget(btn, 0, Qt::AlignRight);
    //vl->setSizeConstraint(QLayout::SetFixedSize);
    about->setLayout(vl);
    about->layout()->setSizeConstraint(QLayout::SetFixedSize);

    about->open();

    about->exec();

    about->deleteLater();
}

void MainWindow::displayImageStats(std::vector<QRectF> rois)
{
    if (input.cols == 0)
        return;

    Mat out, gr;

    if (input.channels() == 3)
        gr = input;
    else
        cvtColor(input, gr, cv::COLOR_GRAY2RGB);

    if (pfmt == "RGB")
        out = gr;
    else if (pfmt == "HSV")
        cvtColor(gr, out, cv::COLOR_RGB2HSV);
    else if (pfmt == "HLS")
        cvtColor(gr, out, cv::COLOR_RGB2HLS);
    else if (pfmt == "Lab")
        cvtColor(gr, out, cv::COLOR_RGB2Lab);
    else if (pfmt == "Luv")
        cvtColor(gr, out, cv::COLOR_RGB2Luv);

    Mat mask = Mat::zeros(input.size(), CV_8UC1);

    for (auto &roi : rois)
    {
        Mat i(mask, Rect(roi.x(), roi.y(), roi.width(), roi.height()));
        i = Scalar(1);
    }
    
    Scalar m, stdv;
    meanStdDev(out, m, stdv, mask);

    if (rois.size() == 1 && rois[0].x() == 0 && rois[0].y() == 0 &&
        rois[0].width() == input.cols && rois[0].height() == input.rows)
        qInfo() << "Image mean color - " << pfmt.toStdString().c_str() << "(" <<
                   m.val[0] << ", " << m.val[1] << ", " << m.val[2] << ")" <<
                   ", color SD - " << pfmt.toStdString().c_str() << "(" <<
                   stdv.val[0] << ", " << stdv.val[1] << ", " << stdv.val[2] << ")";
    else
        qInfo() << "Region-of-interest mean color - " << pfmt.toStdString().c_str() << "(" <<
                    m.val[0] << ", " << m.val[1] << ", " << m.val[2] << ")" <<
                    ", color SD - " << pfmt.toStdString().c_str() << "(" <<
                    stdv.val[0] << ", " << stdv.val[1] << ", " << stdv.val[2] << ")";

}

MainWindow::~MainWindow()
{
}

void MainWindow::loadAnnotation()
{
    if (!zoomtb->isEnabled())
    {
        qCritical() << "Please load an image before loading annotation.";
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Annotation"), 
        QString(), tr("Text file (*.txt)"));
    if (!fileName.isEmpty())
    {
        RoiManager *mgr = RoiManager::GetInstance();
        mgr->loadAnnotation(fileName);
        emit imageChanged();

        auto roiitems = mgr->getROIs();
        GraphicsScene *gscene = static_cast<GraphicsScene *>(view->scene());

        for (auto& item : roiitems)
        {
            gscene->addItem(item);
        }

        qInfo() << "Region-of-interests loaded from file " << fileName;
    }
}

//void MainWindow::saveAnnotation()
//{
//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//        QString(), tr("Text file (*.txt)"));
//
//    if (!fileName.isEmpty())
//    {
//        RoiManager *mgr = RoiManager::GetInstance();
//        mgr->saveAnnotation(fileName.toStdString());
//        qInfo() << "Region-of-interests saved to file " << fileName;
//    }
//}

void MainWindow::viewcontextmenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Clear all", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(reinitfeatureview()));
    contextMenu.addAction(&action1);
    
    QAction action2("Clear row(s)", this);
    connect(&action2, &QAction::triggered, [&]()
    {
        QModelIndexList selection = featureview->selectionModel()->selectedIndexes();
        QList<int> rows;

        for (int i = 0; i < selection.count(); i++)
            if (!rows.contains(selection[i].row()))
                rows.push_back(selection[i].row());
        sort(rows.begin(), rows.end(), greater<>());

        for (int i = 0; i < rows.size(); i++)
        {
            featureitemmodel->removeRows(rows[i], 1);
            featureviewsync.erase(featureviewsync.begin() + rows[i]);
            roisync.erase(roisync.begin() + rows[i]);;
            filenamesync.erase(filenamesync.begin() + rows[i]);;
        }

        /*for (int i = 0; i < selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            featureitemmodel->removeRow()
        }*/
    });

    contextMenu.addAction(&action2);


    contextMenu.exec(featureview->mapToGlobal(pos));
}

void MainWindow::addToolBar(QToolBar *tbar)
{
    QMainWindow::addToolBar(tbar);
    plugintb = tbar;

    if (!zoomtb->isEnabled())
        plugintb->setEnabled(false);
}

void MainWindow::reinitfeatureview()
{
    IPlugin *pl = nullptr;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;

    featureitemmodel->clear();
    featureviewsync.clear();
    roisync.clear();
    filenamesync.clear();

    auto colnames = pl->getCSVColumnNames();
    QStringList collist;

    for (auto& colname : colnames)
        collist.push_back(colname);
    
    featureitemmodel->setHorizontalHeaderLabels(collist);
    colnamelist = collist;
}

void MainWindow::setInitialBanner(QPixmap initbanner, QPixmap orgbanner, string appbanner, string inistr)
{
    QFont f("Arial", 15, QFont::Bold);
    QFont f2("Arial", 22, QFont::Bold);
    
    if (orgbanneritem == nullptr)
    {
        wflowitem = view->scene()->addText(QString::fromStdString(inistr), f);
        appitem = view->scene()->addText(QString::fromStdString(appbanner), f2);

        appbanneritem = view->scene()->addPixmap(initbanner);
        if (!orgbanner.isNull())
            orgbanneritem = view->scene()->addPixmap(orgbanner);
    }

    QPointF pwflow = wflowitem->pos();
    qreal swflow = wflowitem->boundingRect().width(); // QFontMetrics(f).width(QString::fromStdString(inistr)); // wflowitem->textWidth();

    QPointF papp = appitem->pos();
    qreal sapp = appitem->boundingRect().width(); // QFontMetrics(f2).width(QString::fromStdString(appbanner)); // appitem->textWidth();

    QPointF pappbanner = appbanneritem->pos();
    QSize sappbanner = initbanner.size();

    QPointF porgbanner = orgbanneritem->pos();
    QSize sorgbanner = orgbanner.size();

    QRectF scenerect = view->scene()->sceneRect();

    //orgbanneritem->setPos(-(scenerect.width() / 2), (scenerect.height() - sorgbanner.height()));

    //orgbanneritem->setPos(view->mapToScene(QPoint(-sorgbanner.width(), view->height() - sorgbanner.height())));
    if (!orgbanner.isNull())
        orgbanneritem->setPos(view->mapToScene(QPoint(view->width() / 2 - sorgbanner.width() / 2, view->height() - 2 * sorgbanner.height())));
    appbanneritem->setPos(view->mapToScene(QPoint(view->width() / 2 - sappbanner.width() / 2, - sappbanner.height())));
    appitem->setPos(view->mapToScene(QPoint(view->width() / 2 - sapp / 2, QFontMetrics(f2).height())));
    wflowitem->setPos(view->mapToScene(QPoint(view->width() / 2 - swflow / 2, QFontMetrics(f2).height() + 20 + QFontMetrics(f).height())));
}

void MainWindow::copyItems()
{
    QItemSelectionModel * selection = featureview->selectionModel();
    QModelIndexList indexes = selection->selectedIndexes();

    if (indexes.size() == 0)
    {
        qWarning() << "Unknown selection.";
        return;
    }

    QString selected_text;
    // You need a pair of indexes to find the row changes
    QModelIndex previous = indexes.first();
    //indexes.removeFirst();

    for (const QModelIndex &current : indexes)
    {
        QVariant data = featureitemmodel->data(current);
        QString text = data.toString();
        
        if (current.row() != previous.row())
        {
            selected_text.append('\n');
        }
        else if (current.column() != previous.column())
        {
            selected_text.append('\t');
        }

        selected_text.append(text);
        previous = current;
    }

    QApplication::clipboard()->setText(selected_text);
}

ProgressReporter::ProgressReporter(IPlugin *plugin)
{
    pl = plugin;

    lblstatus = new QLabel();

    pbar = new QProgressBar();
    pbar->setMinimum(0);
    pbar->setMaximum(pl->getProgressSteps());
    pbar->setValue(0);
    pbar->setTextVisible(true);
    pbar->setFixedWidth(200);

    QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_BrowserStop);
    //QPixmap map = icn.pixmap(20, 20);

    cbtn = new QPushButton(icn, "");
    connect(cbtn, &QPushButton::clicked, this, &ProgressReporter::cancelProgress);

    worker = new InteractiveExtractorThread();
    worker->setPlugin(pl);
    connect(worker, &QThread::finished, this, &ProgressReporter::finished);
    connect(worker, &InteractiveExtractorThread::ReportProgress, this, &ProgressReporter::UpdateProgress);
    connect(dynamic_cast<QObject *>(pl), SIGNAL(updateProgress(QString)), worker, SLOT(GetProgress(QString)));

    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(lblstatus);

    QHBoxLayout *hl = new QHBoxLayout();
    hl->addWidget(pbar);
    hl->addWidget(cbtn);

    vl->addLayout(hl);
    setLayout(vl);
    setWindowFlags(Qt::FramelessWindowHint);
}

ProgressReporter::~ProgressReporter()
{
    lblstatus->deleteLater();
    pbar->deleteLater();
    cbtn->deleteLater();
    worker->deleteLater();
}

void ProgressReporter::start()
{
    canceled = false;
    worker->start();
}

bool ProgressReporter::wasCanceled()
{
    return canceled;
}

void ProgressReporter::finished()
{
    this->close();
}

void ProgressReporter::cancelProgress()
{
    pl->abort();
    canceled = true;
    while (worker->isRunning())
    {
        QThread::msleep(20);
    }
}

void ProgressReporter::UpdateProgress(QString status)
{
    lblstatus->setText(status);
    pbar->setValue(pbar->value() + 1);
}
