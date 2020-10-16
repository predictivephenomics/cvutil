/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: FeatureExtractorThread.cpp

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

#include "FeatureExtractorThread.h"

//#include <ImageProcessor.h>
#include <PluginManager.h>

#include "../cvutil_core.h"

using namespace std;
using namespace cv;

FeatureExtractorThread::FeatureExtractorThread(QObject *parent) : QThread(parent)
{
    moveToThread(this);
    reset();
}

void FeatureExtractorThread::reset()
{
    fileidx = 0;
}

void FeatureExtractorThread::setfilelist(vector<QString> flist)
{
    filelist = flist;
}

void FeatureExtractorThread::setsavelocation(QString location)
{
    saveloc = location;
}

void FeatureExtractorThread::setMainPlugin(IPlugin *p)
{
    mainplugin = p;
}

// We wanted to implement our own "finished"
// functionality than using the isFinished()
// because the thread we implemented is 
// pausable. The function below returns false
// when the thread is stopped due to a pause
// action.
bool FeatureExtractorThread::isfinished()
{
    return workfinished;
}

void FeatureExtractorThread::run()
{
    workfinished = false;
    Mat inp, out;
    IPlugin *plugin;

    if (!mainplugin)
    {
        PluginManager *p = PluginManager::GetInstance();
        auto plugins = p->GetPlugins();
        plugin = plugins[plugin_index];
    }
    else
        plugin = mainplugin;

    if (plugin->getOutputType() == OutputType::ImageAndValues ||
        plugin->getOutputType() == OutputType::ImagesAndValues)
        plugin->writeHeader(saveloc);

    for (; fileidx < filelist.size(); fileidx++)
    {
        if (isInterruptionRequested())
            return;
        
        emit ReportProgress(filelist[fileidx], fileidx);

        QFileInfo finfo(filelist[fileidx]);

        //qDebug() << "DEBUG :: " << filelist[fileidx];
        inp = cvutil::imread(filelist[fileidx]);
        if (inp.channels() == 3)
            cvtColor(inp, inp, COLOR_BGR2RGB);

        plugin->setImage(inp, finfo.fileName());
        plugin->execute();
        plugin->saveOutput(saveloc, filelist[fileidx]);

        //pfunc(&config, features);
    }

    workfinished = true;
}

