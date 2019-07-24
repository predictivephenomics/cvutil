/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: cvutil_matlab_interface.h

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

#ifndef MATLAB_INTERFACE_H
#define MATLAB_INTERFACE_H

#include "cvutil.h"
#include "figure.h"

#define bwconncomp   getConnectedComponents

namespace cvutil
{
    enum class FindType { Indices, IndicesAndValues, Subscripts, SubscriptsAndValues };

    // MATLAB interface to im2double()
    CVUTILAPI cv::Mat im2double(cv::Mat input);
    CVUTILAPI cv::Mat im2float(cv::Mat input);

    // floor()
    // Round towards negative infinity.
    //
    // Input:
    // m         - Matrix containing float or double values.
    //             If matrix contains integer values, the same matrix is returned.
    // Output:
    // Matrix rounded towards negative infinity. The type of the matrix will be 
    // same as the input matrix.
    CVUTILAPI cv::Mat floor(cv::Mat m);

    // ceil()
    // Round towards positive infinity.
    //
    // Input:
    // m         - Matrix containing float or double values.
    //             If matrix contains integer values, the same matrix is returned.
    // Output:
    // Matrix rounded towards negative infinity. The type of the matrix will be 
    // same as the input matrix.
    CVUTILAPI cv::Mat ceil(cv::Mat m);

    template<typename T>
    std::vector<T> cvutil::unique(const cv::Mat& input, bool sort = true);

    template <typename T>
    std::vector<T> cvutil::range(T _minv, T _maxv = INT_MIN, T step = T(1));

    // TODO: Equivalent to MATLAB imhist.
    CVUTILAPI std::pair<cv::Mat, cv::Mat> imhist(cv::Mat m);

    // ind2sub()
    // Gets subscripts in each dimension.
    // 
    // Input :
    // sz        - Size of the matrix
    // input     - A single column and single channel matrix
    //             indices.
    // nchannels - Number of channels of the matrix, from 
    //             which the indices are present.
    // Output :
    // Returns 2 or 3 column matrix, such that the columns
    // contain row, column and channel subscripts in that order.
    CVUTILAPI cv::Mat ind2sub(cv::Size sz, cv::Mat input, int nchannels = 1);

    // ind2sub()
    // Gets subscripts in each dimension.
    // 
    // Input :
    // sz        - Vector containing the number of rows (or height), 
    //             columns (or width) and channels of the matrix.
    // input     - A single column and single channel matrix
    //             indices.
    // Output :
    // Returns 2 or 3 column matrix, such that the columns
    // contain row, column and channel subscripts in that order.
    CVUTILAPI cv::Mat ind2sub(cv::Vec3i sz, cv::Mat input);

    // sub2ind()
    // Gets linear index from the subscripts in each dimension.
    // 
    // Input :
    // sz        - Size of the matrix
    // input     - A 2 or 3 column and single channel matrix
    //             containing subscripts rows, columns and channels
    //             in that order.
    // nchannels - Number of channels of the matrix, from 
    //             which the indices are present.
    // Output :
    // Returns single column matrix, containing indices.
    CVUTILAPI cv::Mat sub2ind(cv::Size sz, cv::Mat input, int nchannels = 1);
    
    // sub2ind()
    // Gets linear index from the subscripts in each dimension.
    // 
    // Input :
    // sz        - Vector containing the number of rows (or height), 
    //             columns (or width) and channels of the matrix.
    // input     - A 2 or 3 column and single channel matrix
    //             containing subscripts rows, columns and channels
    //             in that order.
    // Output :
    // Returns single column matrix, containing indices.
    CVUTILAPI cv::Mat sub2ind(cv::Vec3i sz, cv::Mat input);

    // find()
    // Find indices/subscripts and values of non-zero elements.
    // 
    // Input :
    // x         - x is a multi-dimensional marix.
    // type      - type specifies what result is to be returned.
    //     Indices             - Returns a single column matrix containing 
    //                           linear indices in x.
    //     IndicesAndValues    - Returns a two column matrix, the first
    //                           column contains indices and the second 
    //                           column contains the value of the element 
    //                           at that location.
    //     Subscripts          - Returns a two or three column matrix,
    //                           containing subscripts for rows, columns
    //                           and channels in that order.
    //     SubscriptsAndValues - Returns a three or four column matrix,
    //                           containing subscripts for rows, columns
    //                           and channels in that order, as well as 
    //                           the last column containing the value of 
    //                           the element at that location.
    // n         - n is the number of first/last indices/subscripts that
    //             needs to be returned. If n is negative, it returns all 
    //             the subscripts/indices that are non-zero.
    // direction - String, when specified to be "first", returns the first 
    //             n indices of the non-zero elements in x. When specified
    //             as "last", returns the last n indices of the non-zero 
    //             elements in x. n value needs to be specified, otherwise
    //             this argument may be ignored.
    // Output :
    // Returns a matrix of type CV_64FC1 with indices/subscripts and/or 
    // values of non-zero elements.
    CVUTILAPI std::pair<cv::Mat, cv::Mat> find(cv::Mat x, FindType type = FindType::Indices, int n = -1, std::string direction = "first");

    CVUTILAPI cv::Mat bwdist(cv::Mat input);
    CVUTILAPI cv::Mat bwdist(cv::Mat input, cv::Mat& label, int labeltype = cv::DistanceTransformLabelTypes::DIST_LABEL_CCOMP);
}


#endif
