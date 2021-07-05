/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: InteractiveExtractorThread.h

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

#pragma once

#ifndef INTERACTIVEEXTRACTORTHREAD_H
#define INTERACTIVEEXTRACTORTHREAD_H

//#include <cvutil.h>

#include <QtWidgets/QDialog>
#include <QtCore/QQueue>
#include <QtCore/QThread>

#include <PluginInterfaces.h>

class InteractiveExtractorThread : public QThread
{
    Q_OBJECT;

    bool workfinished = false;
    IPlugin *mainplugin = nullptr;
public:
    InteractiveExtractorThread(QObject *parent = 0);
    void setPlugin(IPlugin *p);
    void run();

public slots:
    void GetProgress(QString status);

signals:
    void ReportProgress(QString status);
};


#endif
