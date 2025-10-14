/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_templates.h

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

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

#ifndef CVUTIL_TEMPLATES_H
#define CVUTIL_TEMPLATES_H

template<typename T>
std::vector<T> cvutil::unique(const cv::Mat& input, bool sort)
{
    CV_ASSERT2(input.channels() == 1, "input Mat must be a single channel image.");

    std::vector<T> outv;
    for (int y = 0; y < input.rows; ++y)
    {
        const T* row_ptr = input.ptr<T>(y);
        for (int x = 0; x < input.cols; ++x)
        {
            T value = row_ptr[x];

            if (std::find(outv.begin(), outv.end(), value) == outv.end())
                outv.push_back(value);
        }
    }

    if (sort)
        std::sort(outv.begin(), outv.end());

    return outv;
}

// Similar to Python's range function. The parameter minv is 
// inclusive and maxv is exclusive. Hence, we can get a matrix
// containing series range by typecasting using cvutil::getMat()
// which can be handy when making plots.
// Example:
//      vector<int> series = range(0, 5, 2);
//      Mat seriesmat = getMat(series);
//      cout << seriesmat;
// Output:
//      [0;
//      2;
//      4]
// In C++11 or higher, we can use them in range for loops for an
// easier syntax. Unlike the conventional for-loop, which tests 
// a condition before each iteration, the range() function is 
// evaluated only once before the execution of the range for-loop.
// If we have multiple for-loops, we can store the range in a 
// variable for reusability.
// For example:
//      for (auto i : range<double>(10.3, 5.2, -2.7))
//          cout << i << " --- ";
template <typename T>
std::vector<T> cvutil::range(T _minv, T _maxv, T step)
{
    T minv, maxv;

    if (_maxv == INT_MIN)
    {
        minv = 0;
        maxv = _minv;
    }
    else
    {
        minv = _minv;
        maxv = _maxv;
    }

    CV_ASSERT2(((maxv - minv) / step) > 0, "If maxv is greater than minv, then step must be positive or opposite otherwise.");
    std::vector<T> result;
    
    size_t sz = size_t((maxv - minv) / step) + 1;
    result.resize(sz);
    
    for (int i = 0; i < sz; i++)
        result[i] = minv + (T(i) * step);
    
    return result;
}

#endif
