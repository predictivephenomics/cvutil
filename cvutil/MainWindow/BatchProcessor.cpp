/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: BatchProcessor.cpp

Author: Anand Seethepalli (aseethepalli@noble.org)
Assistant Professor: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

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

#include "BatchProcessor.h"

using namespace std;
using namespace cv;

BatchProcessor::BatchProcessor(IPlugin *plugin, QString inputfileloc)
{
    //this->setWindowIcon(QIcon(":/icons/rvanalyzernew"));
    workthread = new FeatureExtractorThread();
    
    // Create new Plugin UI
    if (plugin == nullptr)
    {
        PluginManager *p = PluginManager::GetInstance();
        pwidgets = p->GetPluginUIs();
    }
    else
    {
        mainplugin = plugin;
        workthread->setMainPlugin(mainplugin);
        mainpluginui = mainplugin->getPluginUI();
    }
    
    //auto plugins = p->GetPlugins();

    //for (auto plugin : plugins)
    //{
    //    pwidgets.push_back(new ParameterListWidget(plugin));
    //    //pwidgets[pwidgets.size() - 1]->setWindowModality(Qt::WindowModality::WindowModal);
    //}
    //createMenu();
    imglocGroupBox = createHorizontalGroupBox(tr("Image(s) location"), &imgbtn, "Browse", &imgline, &imglabel, inputfileloc);
    imgsavGroupBox = createHorizontalGroupBox(tr("Output location"), &savbtn, "Browse", &savline, &savlabel);
    
    createOptionsGroupBox();
    createProgressGroupBox();
    
    start = new QPushButton("Start");
    pause = new QPushButton("Pause");
    pause->setDisabled(true);
    quit = new QPushButton("Quit");
    QHBoxLayout *hlay = new QHBoxLayout();
    hlay->setAlignment(Qt::AlignRight);
    hlay->addWidget(start, 0, Qt::AlignRight);
    hlay->addWidget(pause, 0, Qt::AlignRight);
    hlay->addWidget(quit, 0, Qt::AlignRight);

    sbar = new QStatusBar();
    sbar->setSizeGripEnabled(false);

    connectSignalstoSlots();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 3); 
    mainLayout->addWidget(imglocGroupBox);
    mainLayout->addWidget(imgsavGroupBox);
    if (imgoptGroupBox != nullptr)
        mainLayout->addWidget(imgoptGroupBox);
    mainLayout->addWidget(imgproGroupBox);
    mainLayout->addLayout(hlay);
    mainLayout->addWidget(sbar);
    
    setLayout(mainLayout);
    updateLabels();

    //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    
    sbar->showMessage("Ready.", statusbartimeout);
    mode = CurrentMode::Stopped;
}

void BatchProcessor::connectSignalstoSlots()
{
    connect(imgline, &QLineEdit::textChanged, this, &BatchProcessor::updateLabels);
    connect(savline, &QLineEdit::textChanged, this, &BatchProcessor::updateLabels);

    connect(imgbtn, &QPushButton::released, this, &BatchProcessor::browse);
    connect(savbtn, &QPushButton::released, this, &BatchProcessor::browse);

    connect(start, &QPushButton::clicked, this, &BatchProcessor::startclicked);
    connect(pause, &QPushButton::clicked, this, &BatchProcessor::pauseclicked);
    connect(quit, &QPushButton::clicked, this, &BatchProcessor::close);

    connect(workthread, &FeatureExtractorThread::ReportProgress, this, &BatchProcessor::UpdateProgress);
    connect(workthread, &FeatureExtractorThread::finished, this, &BatchProcessor::workfinished);
}

void BatchProcessor::closeEvent(QCloseEvent * ev)
{
    if (mode == CurrentMode::Started || mode == CurrentMode::Resumed || mode == CurrentMode::Paused)
    {
        if (QMessageBox::question(nullptr, "RhizoVision Analyzer", "Do you wish to cancel the progress?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
        {
            realstop();
            ev->accept();
        }
        else
            ev->ignore();
    }
    else
        ev->accept();
}

void BatchProcessor::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key::Key_Escape)
    {
        if (mode == CurrentMode::Started || mode == CurrentMode::Resumed || mode == CurrentMode::Paused)
        {
            if (QMessageBox::question(nullptr, "RhizoVision Analyzer", "Do you wish to cancel the progress?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::StandardButton::Yes)
            {
                ev->accept();
                reject();
            }
            else
                ev->ignore();
        }
        else
        {
            ev->accept();
            reject();
        }
    }
}

void BatchProcessor::getfilelist(QString imgpath)
{
    filelist.clear();

    /*for (auto & p : experimental::filesystem::directory_iterator(imgpath))
    {
        string f = p.path().extension().string();
        transform(f.begin(), f.end(), f.begin(), toupper);

        if (f == ".PNG"  || f == ".JPG"  || f == ".BMP" || 
            f == ".JPEG" || f == ".JPE"  || f == ".DIB" ||
            f == ".JP2"  || f == ".TIFF" || f == ".TIF")
        {
            filelist.push_back(p.path().string());
        }
    }*/

    QDir dir(imgpath);
    QStringList images = dir.entryList(QStringList() 
        << "*.png" << "*.PNG" << "*.jpg" << "*.JPG" << "*.bmp" << "*.BMP" << "*.jpeg" << "*.JPEG" << "*.jpe" << "*.JPE"
        << "*.dib" << "*.DIB" << "*.jp2" << "*.JP2" << "*.tiff" << "*.TIFF" << "*.tif" << "*.TIF", QDir::Files);
    for (auto image : images)
        filelist.push_back(imgpath + image);
}

void BatchProcessor::UpdateProgress(QString filename, int fileno)
{
    string basename = experimental::filesystem::path(filename.toStdString()).filename().string();
    currfile->setText(tr((basename + " (" +
        to_string(fileno + 1) + " of " + to_string(filelist.size()) + ")").c_str()));
    progress->setValue(fileno);
    esttime = elapsed + timer.elapsed();
    nfiles = fileno;
    estcheck = true;
}

void BatchProcessor::workfinished()
{
    // UI changes
    if (workthread->isfinished())
    {
        sbar->showMessage("Process complete", statusbartimeout);
        start->setText("Start");
        imglocGroupBox->setEnabled(true);
        imgsavGroupBox->setEnabled(true);
        if (imgoptGroupBox != nullptr)
            imgoptGroupBox->setEnabled(true);
        pause->setText("Pause");
        pause->setDisabled(true);

        progress->setValue(static_cast<int>(filelist.size()));
        currfile->setText(tr(""));

        workthread->reset();

        // Stop the timer
        elapsed = 0;
        timer.invalidate();

        QTime e(0, 0);
        timeest->setText(e.toString("hh:mm:ss"));

        mode = CurrentMode::Stopped;
    }
}

bool namecompare(string left, string right)
{
    bool pcont = false;
    char *cleft = (char *)left.c_str();
    char *cright = (char *)right.c_str();

    while (1)
    {
        pcont = false;

        if (strlen(cleft) == 0 || strlen(cright) == 0)
            return (strlen(cleft) != 0);
        
        if (isdigit(*cleft) && isdigit(*cright))
        {
            char *rleft;
            char *rright;
            long long valA = strtoll(cleft, &rleft, 10);
            long long valB = strtoll(cright, &rright, 10);

            if (valA != valB)
                return ((valA - valB) < 0);
            else if (int(rright - cright) != int(rleft - cleft))
                return (int(rright - cright) - int(rleft - cleft)) < 0;
            else
            {
                cleft = rleft;
                cright = rright;
                continue;
            }
        }
        if (isdigit(*cleft) || isdigit(*cright))
            return (isdigit(*cleft) != 0);
        
        while (*cleft && *cright)
        {
            if (isdigit(*cleft) || isdigit(*cright))
            {
                pcont = true;
                break;
            }
            if (tolower(*cleft) != tolower(*cright))
                return (tolower(*cleft) - tolower(*cright)) < 0;
            
            cleft++;
            cright++;
        }

        if (pcont)
            continue;

        return (*cleft == '\0');
    }
}

bool BatchProcessor::realstart()
{
    QString imgpath = imgloc;
    std::replace(imgpath.begin(), imgpath.end(), '/', '\\');
    QString savpath = savloc;
    std::replace(savpath.begin(), savpath.end(), '/', '\\');

    // Check for valid paths.
    if (!CanStartProcessing())
    {
        if (!IsValidImageLoc)
            sbar->showMessage("Invalid image location.", statusbartimeout);
        else if (!IsValidSaveLoc)
            sbar->showMessage("Invalid output location.", statusbartimeout);

        return false;
    }

    // Verify if at least one image file exists.
    getfilelist(imgpath);

    if (filelist.size() == 0)
    {
        sbar->showMessage("No image found.", statusbartimeout);
        return false;
    }
    
    IPlugin *pl;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        pl = plugins[pnamelist->currentIndex()];
    }
    else
        pl = mainplugin;

    // UI changes.
    sbar->showMessage("Processing", statusbartimeout);
    start->setText("Stop");
    pause->setEnabled(true);
    imglocGroupBox->setDisabled(true);
    imgsavGroupBox->setDisabled(true);
    if (imgoptGroupBox != nullptr)
        imgoptGroupBox->setDisabled(true);
    progress->setMaximum(static_cast<int>(filelist.size()));

    //sort(filelist.begin(), filelist.end(), namecompare);
    QCollator collator;
    collator.setNumericMode(true);
    sort(filelist.begin(), filelist.end(), collator);

    workthread->setfilelist(filelist);
    workthread->setPluginIndex(curridx);
    pl->saveMetadata(imgpath, savpath);

    // Save the settings in a metadata file in the output directory.
    workthread->start();
    
    // Start the timer
    elapsed = 0;
    timer.restart();
    if (timerid == -1)
        timerid = startTimer(100);
    QTime est(0, 0);
    est = est.addSecs(static_cast<int>(filelist.size()) * 3);
    timeest->setText(est.toString("hh:mm:ss"));
    timeel->setText(QTime(0, 0).toString("hh:mm:ss"));

    return true;
}

void BatchProcessor::realstop()
{
    // UI changes
    start->setText("Start");
    imglocGroupBox->setEnabled(true);
    imgsavGroupBox->setEnabled(true);
    if (imgoptGroupBox != nullptr)
        imgoptGroupBox->setEnabled(true);
    pause->setText("Pause");
    pause->setDisabled(true);
    
    workthread->requestInterruption();
    
    // Wait till thread stopped
    while (!workthread->isFinished())
        Sleep(500);

    Sleep(500);

    // Stop the timer
    elapsed = 0;
    timer.invalidate();
    
    progress->setValue(0);
    workthread->reset();
    currfile->setText(tr(""));
    QTime e(0, 0);
    timeest->setText(e.toString("hh:mm:ss"));
    sbar->showMessage("Stopped", statusbartimeout);
}

void BatchProcessor::realpause()
{
    // UI changes
    pause->setText("Resume");
    if (imgoptGroupBox != nullptr)
        imgoptGroupBox->setEnabled(true);
    
    workthread->requestInterruption();
    
    // Proceed to event handling only 
    // after thread is stopped.
    while (!workthread->isFinished())
        Sleep(500);

    Sleep(500);

    // Timer updating...
    // Pause is similar to stopping except that
    // we store the elapsed time so far.
    elapsed += timer.elapsed();
    timer.invalidate();

    sbar->showMessage("Paused", statusbartimeout);
}

void BatchProcessor::realresume()
{
    // UI changes
    sbar->showMessage("Processing", statusbartimeout);
    pause->setText("Pause");
    if (imgoptGroupBox != nullptr)
        imgoptGroupBox->setDisabled(true);

    workthread->start();

    // Timer update...
    timer.restart();
    if (timerid == -1)
        timerid = startTimer(100);
}

// The same button handler does Start and Stop handling.
// We just dispatch to real event handlers here based on
// current mode. We also use the modes in the real handlers
// to perform additional tasks.
void BatchProcessor::startclicked()
{
    if (mode == CurrentMode::Stopped)
    {
        if (realstart())
            mode = CurrentMode::Started;
    }
    else
    {
        realstop();
        mode = CurrentMode::Stopped;
    }
}

void BatchProcessor::pauseclicked()
{
    if (mode == CurrentMode::Started || mode == CurrentMode::Resumed)
    {
        realpause();
        mode = CurrentMode::Paused;
    }
    else if (mode == CurrentMode::Paused)
    {
        realresume();
        mode = CurrentMode::Resumed;
    }
}

void BatchProcessor::browse()
{
    QPushButton *s = qobject_cast<QPushButton *>(sender());
    /*QFileDialog dlg;
    dlg.setFileMode(QFileDialog::FileMode::DirectoryOnly);*/
    QString fileloc;

    if (s == imgbtn)
    {
        QString filepath = imgline->text();
        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        //experimental::filesystem::path p(filepath);
        QDir p(filepath);

        //if (experimental::filesystem::exists(p) &&
        //    experimental::filesystem::is_directory(p))
        if (p.exists() && filepath.length() > 0)
        {
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Image(s) Location"), QDir(filepath).path(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        }
        else
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Image(s) Location"), QString(), 
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        
        if (fileloc.length() > 0)
            imgline->setText(fileloc);
    }
    else if (s == savbtn)
    {
        QString filepath = savline->text();
        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        //experimental::filesystem::path p(filepath);
        QDir p(filepath);
        
        //if (experimental::filesystem::exists(p) &&
        //    experimental::filesystem::is_directory(p))
        if (p.exists() && filepath.length() > 0)
            fileloc = QFileDialog::getExistingDirectory(this,
                tr("Output Location"), filepath,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        else
        {
            //experimental::filesystem::path ip(imgline->text().toStdString());
            QDir ip(imgline->text());

            //if (experimental::filesystem::exists(ip) &&
            //    experimental::filesystem::is_directory(ip))
            if (ip.exists())
                fileloc = QFileDialog::getExistingDirectory(this,
                    tr("Output Location"), imgline->text(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            else
                fileloc = QFileDialog::getExistingDirectory(this,
                    tr("Output Location"), QString(),
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        }

        if (fileloc.length() > 0)
            savline->setText(fileloc);
    }
}

bool BatchProcessor::CanStartProcessing()
{
    return (IsValidImageLoc && IsValidSaveLoc);
}

void BatchProcessor::createProgressGroupBox()
{
    int fixedwidth = 180;
    imgproGroupBox = new QGroupBox("Progress");
    vlayout = new QVBoxLayout;

    QGridLayout * glayout = new QGridLayout();
    currfilename = new QLabel("Current file : ");
    currfile = new QLineEdit("");
    currfile->setReadOnly(true);
    QPalette readOnlyPalette = currfile->palette();
    QColor mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    currfile->setPalette(readOnlyPalette);
    currfile->setFixedWidth(180);

    timeelapsed = new QLabel("Time elapsed : ");
    timeel = new QLineEdit();
    timeel->setReadOnly(true);
    readOnlyPalette = timeel->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    timeel->setPalette(readOnlyPalette);
    timeel->setFixedWidth(180);

    timeremaining = new QLabel("Est. time remaining : ");
    timeest = new QLineEdit();
    timeest->setReadOnly(true);
    readOnlyPalette = timeest->palette();
    mainWindowBgColor = palette().color(QPalette::Window);
    readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
    timeest->setPalette(readOnlyPalette);
    timeest->setFixedWidth(180);
    
    glayout->addWidget(currfilename, 0, 0);
    glayout->addWidget(currfile, 0, 1, 1, 2, Qt::AlignRight);
    glayout->addWidget(timeelapsed, 1, 0);
    glayout->addWidget(timeel, 1, 1, 1, 2, Qt::AlignRight);
    glayout->addWidget(timeremaining, 2, 0);
    glayout->addWidget(timeest, 2, 1, 1, 2, Qt::AlignRight);

    progress = new QProgressBar();
    progress->setMinimum(0);
    progress->setValue(0);
    progress->setTextVisible(true);
    QHBoxLayout *hlayoutprog = new QHBoxLayout;
    hlayoutprog->addStretch();
    hlayoutprog->addWidget(progress, 1);
    
    vlayout->addLayout(glayout);
    vlayout->addLayout(hlayoutprog, 1);
    
    imgproGroupBox->setLayout(vlayout);
}

void BatchProcessor::updateLabels()
{
    QString filepath = imgline->text();
    //experimental::filesystem::path p(filepath);
    QFileInfo p(filepath);
    int iconheight = 20;

    //if (experimental::filesystem::exists(p) &&
    //    experimental::filesystem::is_directory(p))
    if (p.exists() && p.isDir())
    {
        QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        imglabel->setPixmap(map);
        IsValidImageLoc = true;

        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        imgloc = filepath;
    }
    else
    {
        /*QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogCancelButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        imglabel->setPixmap(map);*/
        imglabel->setPixmap(QPixmap(0, 0));
        IsValidImageLoc = false;
    }

    filepath = savline->text();
    //p = experimental::filesystem::path(filepath);
    p = QFileInfo(filepath);

    //if (experimental::filesystem::exists(p) &&
    //    experimental::filesystem::is_directory(p))
    if (p.exists() && p.isDir())
    {
        QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        savlabel->setPixmap(map);
        IsValidSaveLoc = true;

        if (filepath.length() > 0 && (filepath[filepath.length() - 1] != '/' || filepath[filepath.length() - 1] != '\\'))
            filepath += "/";
        savloc = filepath;
        workthread->setsavelocation(savloc);
    }
    else
    {
        /*QIcon icn = this->style()->standardIcon(QStyle::StandardPixmap::SP_DialogCancelButton);
        QPixmap map = icn.pixmap(iconheight, iconheight);
        savlabel->setPixmap(map);*/
        savlabel->setPixmap(QPixmap(0, 0));
        IsValidSaveLoc = false;
    }
}

QGroupBox *BatchProcessor::createHorizontalGroupBox(QString grouptext, QPushButton **btn, QString btntext, QLineEdit **edt, QLabel **lab, QString inputfileloc)
{
    QGroupBox *result = new QGroupBox(grouptext);
    QHBoxLayout *layout = new QHBoxLayout;

    (*edt) = new QLineEdit(inputfileloc);
    (*btn) = new QPushButton(btntext);
    (*lab) = new QLabel();
    layout->addWidget(*edt);
    layout->addWidget(*btn);
    layout->addWidget(*lab);
    //layout->addStretch();
    result->setLayout(layout);

    return result;
}

void BatchProcessor::createOptionsGroupBox()
{
    if (mainplugin == nullptr)
    {
        imgoptGroupBox = new QScrollArea(this);
        vlayout = new QVBoxLayout;
        pnamelist = new QComboBox();
        PluginManager *p = PluginManager::GetInstance();

        for (auto &pn : p->ListNames())
            pnamelist->addItem(QString::fromStdString(pn));

        if (p->ListNames().size() > 0)
            pnamelist->setCurrentIndex(0);

        QHBoxLayout *hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        QLabel *lb = new QLabel("Algorithm : ");
        hl->addWidget(lb);
        hl->addWidget(pnamelist, 1);

        vlayout->addLayout(hl);
        vlayout->addSpacing(10);

        for (int i = 0; i < pwidgets.size(); i++)
        {
            pwidgets[i]->setVisible(false);
            vlayout->addWidget(pwidgets[i], 0, Qt::Alignment::enum_type::AlignTop);
        }

        pwidgets[0]->setVisible(true);

        connect(pnamelist, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int idx)
        {
            //QLayoutItem *item = vlayout->replaceWidget(pwidgets[curridx], pwidgets[idx]);
            //delete item;
            pwidgets[curridx]->setVisible(false);
            //pwidgets[idx]->setWindowModality(Qt::WindowModality::ApplicationModal);
            pwidgets[idx]->setVisible(true);

            curridx = idx;
            this->update();
        });

        QWidget *w = new QWidget();
        w->setLayout(vlayout);
        imgoptGroupBox->setWidget(w);
        imgoptGroupBox->setFixedHeight(300);
        imgoptGroupBox->setWidgetResizable(true);
        setMinimumWidth(200);
        setFixedHeight(620);
    }
    else if (mainpluginui != nullptr)
    {
        imgoptGroupBox = new QScrollArea(this);
        vlayout = new QVBoxLayout;
        
        vlayout->addWidget(mainpluginui, 0, Qt::Alignment::enum_type::AlignTop);
        QWidget *w = new QWidget();
        w->setLayout(vlayout);
        imgoptGroupBox->setWidget(w);
        imgoptGroupBox->setFixedHeight(300);
        imgoptGroupBox->setWidgetResizable(true);
        setMinimumWidth(200);
        setFixedHeight(620);
    }
}

void BatchProcessor::timerEvent(QTimerEvent * ev)
{
    if (ev->timerId() != timerid)
        return;

    QTime t(0, 0);
    t = t.addMSecs(elapsed);
    if (timer.isValid())
        t = t.addMSecs(timer.elapsed());
    else
    {
        killTimer(timerid);
        timerid = -1;
    }

    if (!workthread->isfinished())
        timeel->setText(t.toString("hh:mm:ss"));

    if (estcheck && nfiles > 0)
    {
        qint64 estimated = qint64(double(double(esttime) / double(nfiles)) *
            double(filelist.size() - nfiles));
        QTime e(0, 0);
        e = e.addMSecs(estimated);
        timeest->setText(e.toString("hh:mm:ss"));

        estcheck = false;
    }
    else if (estcheck)
        estcheck = false;

}
