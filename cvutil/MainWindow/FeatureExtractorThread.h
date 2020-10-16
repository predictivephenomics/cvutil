/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: FeatureExtractorThread.h

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

#ifndef FEATUREEXTRACTORTHREAD_H
#define FEATUREEXTRACTORTHREAD_H

//#include <cvutil.h>

#include <QtWidgets/QDialog>
#include <QtCore/QQueue>
#include <QtCore/QThread>

#include <experimental/filesystem>
#include <PluginInterfaces.h>

class FeatureExtractorThread : public QThread
{
    Q_OBJECT;

    // Location information
    std::vector<QString> filelist;
    int fileidx;
    QString saveloc;

    int plugin_index;
    bool workfinished = false;
    IPlugin *mainplugin = nullptr;
public:
    FeatureExtractorThread(QObject *parent = 0);
    void reset();
    void setfilelist(std::vector<QString> flist);
    void setsavelocation(QString location);
    
    void setMainPlugin(IPlugin *p);
    void setPluginIndex(int idx) { plugin_index = idx; }
    bool isfinished();

    void run();

signals:
    void ReportProgress(QString filename, int fileno);
};


#endif
