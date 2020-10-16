/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: cvutil_core.h

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

#ifndef CVUTIL_CORE_H
#define CVUTIL_CORE_H

#include "cvutil.h"

namespace cvutil
{
    CVUTILAPI cv::Mat getSingleChannel(cv::Mat m);
    CVUTILAPI cv::Mat getSingleChannel(cv::Mat m, int type);
    CVUTILAPI cv::Mat getEdgeDetect(cv::Mat img, int lim);

    // getLargestConnectedComponent()
    // Returns largest connected component from a binary image.
    // 
    // Input :
    // img  -  Single or 3-channel binary image of type
    //         CV_8SC1 or CV_8SC3.
    // Output :
    //      Mat object of the same size as img containing
    //      binary image of the largest connected component.
    CVUTILAPI cv::Mat getLargestConnectedComponent(cv::Mat img);

    // getConnectedComponents()
    // Gets connected components from a binary image.
    // 
    // Input :
    // img  -  Single or 3 channel binary image of type
    //         CV_8SC1 or CV_8SC3.
    // size -  Minimum number of pixels needed in the component
    //         for the component to be returned in the list.
    //         Default value is negative, means all the components
    //         are returned in the list.
    // Output :
    //      Vector of vectors containing the 1-D pixel indices of 
    //      connected components. 
    CVUTILAPI std::vector<std::vector<int>> getConnectedComponents(cv::Mat img, int size = -1, int conn = 8);
    CVUTILAPI std::vector<std::vector<int>> getConnectedComponents(cv::Mat img, cv::Mat& lab, int size = -1, int conn = 8);

    CVUTILAPI void window(cv::String winname, cv::Mat m);
    CVUTILAPI void printheader(cv::Mat m);

    CVUTILAPI cv::Mat bwthin(cv::Mat input);
    CVUTILAPI cv::Mat bwskel(cv::Mat input, cv::Mat distance = cv::Mat());

    enum class LineSimplificationType { DouglasPeucker, NPoint };

    CVUTILAPI cv::Mat linesim(cv::Mat input, LineSimplificationType type = LineSimplificationType::DouglasPeucker, double epsilon = 1.0);

    // The following function differs from InputArray::getMat() 
    // in that it converts the vectors into column matrices and vector of
    // vectors and vector of matrices into a single horizontally 
    // concatenated matrix. Hence, the vector of vectors and vector of
    // Matrices must have the same number of elements and rows respectively.
    CVUTILAPI cv::Mat getMat(cv::InputArray x);

    CVUTILAPI cv::Mat addBorder(cv::Mat input, cv::Scalar color, int thickness = 1);

    CVUTILAPI cv::Mat removeBorder(cv::Mat input, int thickness = 1);

    CVUTILAPI void ForEachFileInPath(std::string path, void(*func)(std::string filename));

    CVUTILAPI void init(int argc, char *argv[], bool useOpt = true);
    
    CVUTILAPI void drawText(cv::Mat &GeomLayer, const std::string & text, cv::Point org, cv::Scalar color, int rightmargin, int thickness);

    CVUTILAPI cv::Mat getImageFromComponents(cv::Size sz, std::vector<std::vector<int>> components);

    // Similar to OpenCV's imread, but supports UTF-8 characters in file path.
    CVUTILAPI cv::Mat imread(QString& filename, int flags = cv::IMREAD_COLOR);

    CVUTILAPI bool imwrite(QString filename, cv::Mat img, const std::vector< int > &  	params = std::vector< int >());
    //CVUTILAPI void setSwitch(bool on);
}

#endif


