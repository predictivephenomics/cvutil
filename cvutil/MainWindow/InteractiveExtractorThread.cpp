/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: InteractiveExtractorThread.cpp

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

#include "InteractiveExtractorThread.h"

//#include <ImageProcessor.h>
#include <PluginManager.h>

using namespace std;
using namespace cv;

InteractiveExtractorThread::InteractiveExtractorThread(QObject *parent) : QThread(parent)
{
    moveToThread(this);
}

void InteractiveExtractorThread::setPlugin(IPlugin *p)
{
    mainplugin = p;
}

void InteractiveExtractorThread::run()
{
    if (!mainplugin)
        return;

    //if (isInterruptionRequested())
    //    return;
    
    mainplugin->execute();
    //emit ReportProgress(tr(filelist[fileidx].c_str()), fileidx);
}

void InteractiveExtractorThread::GetProgress(QString status)
{
    emit ReportProgress(status);
}

