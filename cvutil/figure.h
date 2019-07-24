/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: figure.h

Author: Anand Seethepalli (aseethepalli@noble.org)
Principal Investigator: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of Computer Vision UTILity toolkit (cvutil)

cvutil is free software: you can redistribute it and/or modify
it under the terms of the NOBLE RESEARCH INSTITUTE, GENERAL PUBLIC LICENSE.

cvutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
NOBLE RESEARCH INSTITUTE GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the Noble Research Institute General Public License
along with cvutil.  If not, see <https://github.com/noble-research-institute/cvutil/blob/master/LICENSE>.
*/

#pragma once

#ifndef FIGURE_H
#define FIGURE_H

#include "cvutil.h"

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
        virtual void bar(cv::InputArray x, cv::InputArray y = cv::noArray(), std::vector<std::string> groups = std::vector<std::string>()) = 0;
        virtual void bar(std::vector<std::string> &x, cv::InputArray y, std::vector<std::string> groups = std::vector<std::string>()) = 0;

        virtual void scatter(cv::InputArray x, cv::InputArray y = cv::noArray(), std::vector<std::string> groups = std::vector<std::string>()) = 0;

        virtual void subplot(int rows, int cols, int nplot) = 0;

        // Display image
        virtual void imshow(cv::Mat &img) = 0;

        // Titles and labels
        virtual void title(std::string txt) = 0;
        virtual void xlabel(std::string lab) = 0;
        virtual void ylabel(std::string lab) = 0;
        virtual void showgrid(bool on) = 0;
        
        virtual void wait() = 0;

        virtual void release() = 0;
    };

    CVUTILAPI cv::Ptr<figure> createfigure();
    CVUTILAPI cv::Ptr<figure> createfigure(int n);
}

#endif
