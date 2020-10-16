/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: BatchProcessor.h

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

#pragma once

#ifndef BATCHPROCESSOR_H
#define BATCHPROCESSOR_H

//#include <ImageProcessor.h>

#include <PluginInterfaces.h>
#include <PluginManager.h>

//#include <RoiManager.h>

//#include "ParameterListWidget.h"
#include "FeatureExtractorThread.h"

class BatchProcessor : public QDialog
{
    Q_OBJECT;

public:
    BatchProcessor(IPlugin *plugin = nullptr, QString inputfileloc = "");
    
public slots:
    void UpdateProgress(QString filename, int fileno);
    void workfinished();

protected:
    void timerEvent(QTimerEvent * ev);
    void closeEvent(QCloseEvent * ev);
    void keyPressEvent(QKeyEvent *ev);

private:
    //void createMenu();
    QGroupBox *createHorizontalGroupBox(QString grouptext, QPushButton **btn, QString btntext, QLineEdit **edt, QLabel **lab, QString inputfileloc = "");
    void updateLabels();
    void browse();
    bool CanStartProcessing();
    void createOptionsGroupBox();
    void createProgressGroupBox();
    void connectSignalstoSlots();
    void startclicked();
    void pauseclicked();
    bool realstart();
    void realstop();
    void realpause();
    void realresume();
    
    void getfilelist(QString imgpath);
    
    std::vector<QWidget *> pwidgets;
    IPlugin *mainplugin;
    QWidget *mainpluginui;

    QGroupBox *imglocGroupBox;
    QGroupBox *imgsavGroupBox;
    QScrollArea *imgoptGroupBox = nullptr;
    QGroupBox *imgproGroupBox;

    QPushButton *imgbtn, *savbtn, *processbtn;
    QLineEdit *imgline, *savline;
    QLabel *imglabel, *savlabel;

    QVBoxLayout *vlayout;
    
    int curridx = 0;
    QComboBox *pnamelist = nullptr;
    QLabel *currfilename, *timeremaining, *timeelapsed;
    QLineEdit *currfile, *timeel, *timeest;
    QProgressBar *progress;

    QPushButton *start, *pause, *quit;
    QStatusBar *sbar;

    // Timer functionality
    QElapsedTimer timer;
    int timerid = -1;
    qint64 elapsed = 0;
    qint64 esttime = 0;
    int nfiles = 0;
    bool estcheck = false;

    bool IsValidImageLoc = false, IsValidSaveLoc = false;
    QString imgloc, savloc;
    std::vector<QString> filelist;
    
    // Threading functionality
    FeatureExtractorThread *workthread;
    
    const int statusbartimeout = 0;

    enum class CurrentMode { Stopped, Started, Paused, Resumed };
    CurrentMode mode;
};

#endif // DIALOG_H

