/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: InteractiveExtractorThread.cpp

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

