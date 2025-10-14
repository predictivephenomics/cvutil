/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: BatchProcessor.h

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
    QStringList filelist;
    
    // Threading functionality
    FeatureExtractorThread *workthread;
    
    const int statusbartimeout = 0;

    enum class CurrentMode { Stopped, Started, Paused, Resumed };
    CurrentMode mode;
};

#endif // DIALOG_H

