/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: FeatureExtractorThread.h

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

#ifndef FEATUREEXTRACTORTHREAD_H
#define FEATUREEXTRACTORTHREAD_H

//#include <cvutil.h>

#include <QtWidgets/QDialog>
#include <QtCore/QQueue>
#include <QtCore/QThread>

#include <filesystem>
#include <PluginInterfaces.h>

class FeatureExtractorThread : public QThread
{
    Q_OBJECT;

    // Location information
    QStringList filelist;
    int fileidx;
    QString saveloc;

    int plugin_index;
    bool workfinished = false;
    IPlugin *mainplugin = nullptr;
public:
    FeatureExtractorThread(QObject *parent = 0);
    void reset();
    void setfilelist(QStringList flist);
    void setsavelocation(QString location);
    
    void setMainPlugin(IPlugin *p);
    void setPluginIndex(int idx) { plugin_index = idx; }
    bool isfinished();

    void run();

signals:
    void ReportProgress(QString filename, int fileno);
};


#endif
