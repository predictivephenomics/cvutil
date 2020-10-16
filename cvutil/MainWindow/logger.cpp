/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: logger.cpp

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

#include "logger.h"

void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QString lastmsg;
    QString helper = "";

#ifdef _DEBUG
    std::string msgstr = msg.toStdString();
#endif

    if (MainWindow::logbox == 0)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type)
        {
        case QtDebugMsg:
            if (MainWindow::showDebugMessages)
                fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
        }
    }
    else //if (QThread::currentThreadId() == QApplication::instance()->thread())
    {
        switch (type)
        {
        case QtDebugMsg:
            if (MainWindow::showDebugMessages)
            {
                MainWindow::logbox->setTextColor(QColor(0x2C, 0xB5, 0xE9));
                //MainWindow::logbox->append(msg);
                //lastmsg = msg;
                //return;
            }
            else
                return;
            break;
        case QtInfoMsg:
            MainWindow::logbox->setTextColor(QColor(0x0, 0x0, 0x0));
            break;
        case QtWarningMsg:
            MainWindow::logbox->setTextColor(QColor(255, 127, 0));
            break;
        case QtCriticalMsg:
            MainWindow::logbox->setTextColor(QColor("red"));
            helper = "Error: ";
            break;
        case QtFatalMsg:
            abort();
        }

        if (MainWindow::logbox != 0 /*&& lastmsg != msg*/)
        {
            MainWindow::logbox->append(helper + msg);
            lastmsg = msg;
            MainWindow::logbox->setFocus();
        }
    }
    /*else
    {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type)
        {
        case QtDebugMsg:
            if (MainWindow::showDebugMessages)
                fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stdout, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stdout, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stdout, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
        }
    }*/
}

