/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: figure.h

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

#ifndef FIGURE_H
#define FIGURE_H

#include "cvutil.h"
#include <PluginInterfaces.h>
#include <QtWidgets/QtWidgets>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE;
using namespace cvutil;

namespace cvutil
{
    class CVUTILAPI figure
    {
    public:
        // Configure how the plot should look before calling plotting
        // functions.
        virtual void setprecision(int p) = 0;
        
        // Plotting functions
        virtual void bar(cv::InputArray x, cv::InputArray y = cv::noArray(), std::vector<std::string> groups = std::vector<std::string>(), std::string style = "grouped") = 0;
        virtual void bar(std::vector<std::string> &x, cv::InputArray y, std::vector<std::string> groups = std::vector<std::string>(), std::string style = "grouped") = 0;

        virtual void scatter(cv::InputArray x, cv::InputArray y = cv::noArray(), std::vector<std::string> groups = std::vector<std::string>()) = 0;

        virtual void subplot(int rows, int cols, int nplot) = 0;

        // Display image
        virtual void imshow(cv::Mat &img) = 0;

        // Titles and labels
        virtual void title(std::string txt) = 0;
        virtual void xlabel(std::string lab, int fontSize = 13, std::string fontWeight = "normal") = 0;
        virtual void ylabel(std::string lab, int fontSize = 13, std::string fontWeight = "normal") = 0;
        virtual void showgrid(bool on) = 0;
        
        virtual QChart* getChart() = 0;

        virtual void hold(bool on) = 0;
        virtual void show() = 0;
        virtual void wait() = 0;

        virtual void release() = 0;
    };

    CVUTILAPI cv::Ptr<figure> createfigure();
    CVUTILAPI cv::Ptr<figure> createfigure(int n);

    //enum class DisplayMode { InputImage, OutputImage };

    class CVUTILAPI cvutilWindow
    {
    public:
        virtual void show() = 0;
        virtual void loadPlugins(IPlugin *plugin = nullptr) = 0;
        virtual void refreshPlugins() = 0;
        //virtual void setActionWidget(QWidget *w) = 0;
        virtual void addNewDockWidget(QDockWidget *w) = 0;
        virtual void addToolBar(QToolBar *tbar) = 0;

        virtual void enableROI(bool on) = 0;
        virtual void enableActions(bool on) = 0;

        virtual void setVisibleROI(bool on) = 0;
        virtual void setVisibleActions(bool on) = 0;

        virtual void setImage(cv::Mat m) = 0;
        virtual cv::Mat getLoadedImage() = 0;
        virtual int getDisplayOutputIndex() = 0;
        virtual void setDisplayOutputIndex(int idx) = 0;
        virtual int getRotation() = 0;
        virtual bool colorInversionEnabled() = 0;
        virtual void setColorInversionEnabled(bool on) = 0;

        // To get information if the mainwindow is showing input or output image,
        // so the plugin can make decisions for better user interactivity.
        //virtual DisplayMode getDisplayMode() = 0;

        virtual void showChartWidget(QChart *chart) = 0;

        virtual void setAboutText(std::string txt, QPixmap applicationimage, QIcon applicationicon) = 0;
        virtual void setInitialBanner(QPixmap initbanner, QPixmap orgbanner, std::string appbanner, std::string inistr) = 0;
    };

    // A general purpose Main Window for loading images and processing them
    CVUTILAPI cv::Ptr<cvutilWindow> getImageProcessorWindow(QIcon appico = QIcon());

}

#endif
