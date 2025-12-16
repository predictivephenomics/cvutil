/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_matlab_interface.cpp

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

#include "cvutil.h"
#include "cvutil_matlab_interface.h"
#include "cvutil_bwdist.h"

#pragma warning(disable : 4752)

using namespace std;
using namespace cv;

Mat cvutil::im2double(Mat input)
{
    Mat out;

    if (input.channels() == 3)
        input.convertTo(out, CV_64FC3);
    else
        input.convertTo(out, CV_64FC1);

    double min_val, max_val;
    minMaxLoc(out, &min_val, &max_val);

    if (max_val > 1.0)
        out /= max_val;

    return out;
}

Mat cvutil::im2float(Mat input)
{
    Mat out;
    
    if (input.channels() == 3)
        input.convertTo(out, CV_32FC3);
    else
        input.convertTo(out, CV_32FC1);

    double min_val, max_val;
    minMaxLoc(out, &min_val, &max_val);

    if (static_cast<float>(max_val) > 1.0f)
        out /= static_cast<float>(max_val);

    return out;
}

namespace FloorCeilFunctions
{
    Mat floorf(Mat m)
    {
        Mat result = m.clone();
        float *data = result.ptr<float>();
        int nelements = m.rows * m.cols * m.channels();
        __m256 buffer;
        int i = 0, k = 0;
        int stepsize = 8, counter_end = nelements;

        if (checkHardwareSupport(CPU_AVX2))
        {
            // Check for memory alignment before performing vector operations.
            if (((unsigned long long)data) & 0x1f)
            {
                for (; i <= 0x1f && i <= nelements; i++)
                {
                    if (!(((unsigned long long)&data[i]) & 0x1f))
                        break;
                    else
                        data[i] = floor(data[i]);
                }
            }

            for (; i < counter_end; i += stepsize)
            {
                // Vector acceleration for aligned memory.
                if ((i + stepsize) <= counter_end)
                {
                    buffer = _mm256_load_ps(&data[i]);
                    buffer = _mm256_floor_ps(buffer);
                    _mm256_store_ps(&data[i], buffer);
                }
                else // Normal code for the last unaligned elements.
                    for (k = 0; k < (counter_end - i); k++)
                        data[i + k] = floor(data[i + k]);
            }
        }
        else
        {
            for (; i < counter_end; i++)
                data[i] = floor(data[i]);
        }

        return result;
    }

    Mat floorlf(Mat m)
    {
        Mat result = m.clone();
        double *data = result.ptr<double>();
        int nelements = m.rows * m.cols * m.channels();
        __m256d buffer;
        int i = 0, k = 0;
        int stepsize = 4, counter_end = nelements;

        if (checkHardwareSupport(CPU_AVX2))
        {
            // Check for memory alignment before performing vector operations.
            if (((unsigned long long)data) & 0x1f)
            {
                for (; i <= 0x1f && i <= nelements; i++)
                {
                    if (!(((unsigned long long)&data[i]) & 0x1f))
                        break;
                    else
                        data[i] = floor(data[i]);
                }
            }

            for (; i < counter_end; i += stepsize)
            {
                // Vector acceleration for aligned memory.
                if ((i + stepsize) <= counter_end)
                {
                    buffer = _mm256_load_pd(&data[i]);
                    buffer = _mm256_floor_pd(buffer);
                    _mm256_store_pd(&data[i], buffer);
                }
                else // Normal code for the last unaligned elements.
                    for (k = 0; k < (counter_end - i); k++)
                        data[i + k] = floor(data[i + k]);
            }
        }
        else
        {
            for (; i < counter_end; i++)
                data[i] = floor(data[i]);
        }
        
        return result;
    }

    Mat ceilf(Mat m)
    {
        Mat result = m.clone();
        float *data = result.ptr<float>();
        int nelements = m.rows * m.cols * m.channels();
        __m256 buffer;
        int i = 0, k = 0;
        int stepsize = 8, counter_end = nelements;

        if (checkHardwareSupport(CPU_AVX2))
        {
            // Check for memory alignment before performing vector operations.
            if (((unsigned long long)data) & 0x1f)
            {
                for (; i <= 0x1f && i <= nelements; i++)
                {
                    if (!(((unsigned long long)&data[i]) & 0x1f))
                        break;
                    else
                        data[i] = ceil(data[i]);
                }
            }

            for (; i < counter_end; i += stepsize)
            {
                // Vector acceleration for aligned memory.
                if ((i + stepsize) <= counter_end)
                {
                    buffer = _mm256_load_ps(&data[i]);
                    buffer = _mm256_ceil_ps(buffer);
                    _mm256_store_ps(&data[i], buffer);
                }
                else // Normal code for the last unaligned elements.
                    for (k = 0; k < (counter_end - i); k++)
                        data[i + k] = ceil(data[i + k]);
            }
        }
        else
        {
            for (; i < counter_end; i++)
                data[i] = ceil(data[i]);
        }

        return result;
    }

    Mat ceillf(Mat m)
    {
        Mat result = m.clone();
        double *data = result.ptr<double>();
        int nelements = m.rows * m.cols * m.channels();
        __m256d buffer;
        int i = 0, k = 0;
        int stepsize = 4, counter_end = nelements;

        if (checkHardwareSupport(CPU_AVX2))
        {
            // Check for memory alignment before performing vector operations.
            if (((unsigned long long)data) & 0x1f)
            {
                for (; i <= 0x1f && i <= nelements; i++)
                {
                    if (!(((unsigned long long) &data[i]) & 0x1f))
                        break;
                    else
                        data[i] = ceil(data[i]);
                }
            }

            for (; i < counter_end; i += stepsize)
            {
                // Vector acceleration for aligned memory.
                if ((i + stepsize) <= counter_end)
                {
                    buffer = _mm256_load_pd(&data[i]);
                    buffer = _mm256_ceil_pd(buffer);
                    _mm256_store_pd(&data[i], buffer);
                }
                else // Normal code for the last unaligned elements.
                    for (k = 0; k < (counter_end - i); k++)
                        data[i + k] = ceil(data[i + k]);
            }
        }
        else
        {
            for (; i < counter_end; i++)
                data[i] = ceil(data[i]);
        }

        return result;
    }
}

Mat cvutil::floor(Mat m)
{
    if (m.depth() != CV_32F && m.depth() != CV_64F)
        return m.clone();

    if (m.depth() == CV_32F)
        return FloorCeilFunctions::floorf(m);
    else
        return FloorCeilFunctions::floorlf(m);
}

Mat cvutil::ceil(Mat m)
{
    if (m.depth() != CV_32F && m.depth() != CV_64F)
        return m.clone();

    if (m.depth() == CV_32F)
        return FloorCeilFunctions::ceilf(m);
    else
        return FloorCeilFunctions::ceillf(m);
}

pair<Mat, Mat> cvutil::imhist(Mat src)
{
    int hbins = 256;
    int histSize[] = { hbins };
    // hue varies from 0 to 179, see cvtColor
    float hranges[] = { 0, 255 };
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    const float* ranges[] = { hranges };
    MatND hist;
    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = { 0 };

    if (src.depth() == CV_8U || src.depth() == CV_16U || src.depth() == CV_32F)
        cout << "Valid depths..\n";

    calcHist(&src, 1, channels, Mat(), // do not use mask
        hist, 1, histSize, ranges,
        true, // the histogram is uniform
        false);
    double maxVal = 0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);

    cout << "Hist : \n" << hist << endl;

    int scale = 15;
    Mat histImg = Mat::zeros(scale * hbins, hbins * scale + 100, CV_8UC3);
    for (int h = 0; h < hbins; h++)
        //for (int s = 0; s < 1; s++)
    {
        float binVal = hist.at<float>(h, 0); // s);
        double intensity = (binVal * 255.0) / maxVal;
        cout << "start (" << h << ") : " << Point(h*scale, 0) << " --- stop (" << h << ") : " << Point((h + 1)*scale - 1, cvRound(intensity * 10.0)) << endl;
        rectangle(histImg, Point(h*scale, 0), // s*scale),
            Point((h + 1)*scale - 1, cvRound(intensity * scale)), //(s + 1)*scale - 1),
            Scalar::all(255),
            FILLED);
    }
    return pair<Mat, Mat>();
}

Mat cvutil::ind2sub(Size sz, Mat _input, int nchannels)
{
    CV_ASSERT2(_input.channels() == 1 && _input.cols == 1, "_input must be single channel and single column matrix.");

    Mat input;
    if (_input.type() != CV_32SC1)
        _input.convertTo(input, CV_32SC1);
    else
        input = _input.clone();

    input.convertTo(input, CV_32FC1);
    Mat nrows = floor(input / (sz.width * nchannels));
    Mat rem = input - nrows * (sz.width * nchannels);
    Mat ncols = floor(rem / (nchannels));
    Mat nchan = rem - ncols * nchannels;

    vector<Mat> cols = {nrows, ncols, nchan};
    Mat result;
    hconcat(cols, result);

    return result;
}

Mat cvutil::ind2sub(Vec3i sz, Mat _input)
{
    return ind2sub(Size(sz[1], sz[0]), _input, sz[3]);
}

Mat cvutil::sub2ind(Size sz, Mat input, int nchannels)
{
    return sub2ind(Vec3i(sz.height, sz.width, nchannels), input);
}

Mat cvutil::sub2ind(Vec3i sz, Mat _input)
{
    CV_ASSERT2(_input.channels() == 1 && (_input.cols == 2 || _input.cols == 3), "_input must be single channel and have two or three columns.");

    Mat input;
    if (_input.type() != CV_32SC1)
        _input.convertTo(input, CV_32SC1);
    else
        input = _input.clone();

    int nchannels = 0;

    if (_input.cols == 2)
        nchannels = 1;
    else
        nchannels = sz[2];

    Mat result;
    
    if (_input.cols == 2)
        result = _input.col(0) * sz[1] + _input.col(1);
    else
        result = _input.col(0) * (sz[1] * nchannels) + _input.col(1) * nchannels + _input.col(2);

    return result;
}

namespace FindHelper
{
    namespace internal
    {
        template<typename T>
        inline void find_indices(T* inpptr, int* indsubptr, int nelements, int nNonZeros,  int forward)
        {
            const T zero = static_cast<T>(0);

            if (forward)
            {
                for (int i = 0, j = 0; (i < nelements && j < nNonZeros); i++)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j] = i;
                        j++;
                    }
                }
            }
            else
            {
                for (int i = nelements - 1, j = 0; (i >= 0 && j < nNonZeros); i--)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j] = i;
                        j++;
                    }
                }
            }
        }

        template<typename T>
        inline void find_indices_and_values(T* inpptr, T* valptr, int* indsubptr, int nelements, int nNonZeros, int forward)
        {
            const T zero = static_cast<T>(0);

            if (forward)
            {
                for (int i = 0, j = 0; (i < nelements && j < nNonZeros); i++)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j] = i;
                        valptr[j] = inpptr[i];
                        j++;
                    }
                }
            }
            else
            {
                for (int i = nelements - 1, j = 0; (i >= 0 && j < nNonZeros); i--)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j] = i;
                        valptr[j] = inpptr[i];
                        j++;
                    }
                }
            }
        }

        template<typename T>
        inline void find_subscripts(T* inpptr, int* indsubptr, int nelements, int nNonZeros, int forward, int ncols, int nchannels)
        {
            const T zero = static_cast<T>(0);
            int nOutputCols = 2;

            if (nchannels > 1)
                nOutputCols++;

            if (forward)
            {
                for (int i = 0, j = 0; (i < nelements && j < nNonZeros); i++)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j * nOutputCols] = (i / (ncols * nchannels));
                        indsubptr[j * nOutputCols + 1] = (i % (ncols * nchannels)) / nchannels;

                        /// Statement: "(i % (inp.cols * nchannels)) % nchannels" is identical to "i % nchannels"
                        /// Or, "(i % (ab)) % b" is identical to "i % b"
                        /// Proof:
                        /// Let i = ab + c | c < ab
                        /// LHS = (i % (ab)) % b
                        ///     = ((ab + c) % (ab)) % b
                        ///     = c % b
                        ///     = (ab + c) % b
                        ///     = i % b
                        ///     = RHS
                        // 
                        // Thus the following if block is commented and rewritten.

                        //if (nchannels > 1)
                        //    resultptr[j * nOutputCols + 2] = (i % (inp.cols * nchannels)) % nchannels;
                        if (nchannels > 1)
                            indsubptr[j * nOutputCols + 2] = (i % nchannels);
                        j++;
                    }
                }
            }
            else
            {
                for (int i = nelements - 1, j = 0; (i >= 0 && j < nNonZeros); i--)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j * nOutputCols] = (i / (ncols * nchannels));
                        indsubptr[j * nOutputCols + 1] = (i % (ncols * nchannels)) / nchannels;

                        if (nchannels > 1)
                            indsubptr[j * nOutputCols + 2] = (i % nchannels);
                        j++;
                    }
                }
            }
        }

        template<typename T>
        inline void find_subscripts_and_values(T* inpptr, T* valptr, int* indsubptr, int nelements, int nNonZeros, int forward, int ncols, int nchannels)
        {
            const T zero = static_cast<T>(0);
            int nOutputCols = 2;

            if (nchannels > 1)
                nOutputCols++;

            if (forward)
            {
                for (int i = 0, j = 0; (i < nelements && j < nNonZeros); i++)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j * nOutputCols] = (i / (ncols * nchannels));
                        indsubptr[j * nOutputCols + 1] = (i % (ncols * nchannels)) / nchannels;

                        if (nchannels > 1)
                            indsubptr[j * nOutputCols + 2] = i % nchannels;
                        
                        valptr[j] = inpptr[i];
                        j++;
                    }
                }
            }
            else
            {
                for (int i = nelements - 1, j = 0; (i >= 0 && j < nNonZeros); i--)
                {
                    if (inpptr[i] != zero)
                    {
                        indsubptr[j * nOutputCols] = (i / (ncols * nchannels));
                        indsubptr[j * nOutputCols + 1] = (i % (ncols * nchannels)) / nchannels;

                        if (nchannels > 1)
                            indsubptr[j * nOutputCols + 2] = i % nchannels;

                        valptr[j] = inpptr[i];
                        j++;
                    }
                }
            }
        }
    }

    inline void find_indices(Mat input, Mat indsub, int forward)
    {
        void *inpptr;
        int *indsubptr = indsub.ptr<int>();

        switch (input.depth())
        {
        case CV_8U:
            inpptr = (void *)input.ptr<uchar>();
            internal::find_indices<uchar>(static_cast<uchar *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_8S:
            inpptr = (void *)input.ptr<char>();
            internal::find_indices<char>(static_cast<char *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_16U:
            inpptr = (void *)input.ptr<ushort>();
            internal::find_indices<ushort>(static_cast<ushort *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_16S:
            inpptr = (void *)input.ptr<short>();
            internal::find_indices<short>(static_cast<short *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_32S:
            inpptr = (void *)input.ptr<int>();
            internal::find_indices<int>(static_cast<int *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_32F:
            inpptr = (void *)input.ptr<float>();
            internal::find_indices<float>(static_cast<float *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_64F:
            inpptr = (void *)input.ptr<double>();
            internal::find_indices<double>(static_cast<double *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        }
    }

    inline void find_indices_and_values(Mat input, Mat indsub, Mat val, int forward)
    {
        void *inpptr, *valptr;
        int *indsubptr = indsub.ptr<int>();

        switch (input.depth())
        {
        case CV_8U:
            inpptr = (void *)input.ptr<uchar>();
            valptr = (void *)val.ptr<uchar>();
            internal::find_indices_and_values<uchar>(static_cast<uchar *>(inpptr), static_cast<uchar *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_8S:
            inpptr = (void *)input.ptr<char>();
            valptr = (void *)val.ptr<char>();
            internal::find_indices_and_values<char>(static_cast<char *>(inpptr), static_cast<char *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_16U:
            inpptr = (void *)input.ptr<ushort>();
            valptr = (void *)val.ptr<ushort>();
            internal::find_indices_and_values<ushort>(static_cast<ushort *>(inpptr), static_cast<ushort *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_16S:
            inpptr = (void *)input.ptr<short>();
            valptr = (void *)val.ptr<short>();
            internal::find_indices_and_values<short>(static_cast<short *>(inpptr), static_cast<short *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_32S:
            inpptr = (void *)input.ptr<int>();
            valptr = (void *)val.ptr<int>();
            internal::find_indices_and_values<int>(static_cast<int *>(inpptr), static_cast<int *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_32F:
            inpptr = (void *)input.ptr<float>();
            valptr = (void *)val.ptr<float>();
            internal::find_indices_and_values<float>(static_cast<float *>(inpptr), static_cast<float *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        case CV_64F:
            inpptr = (void *)input.ptr<double>();
            valptr = (void *)val.ptr<double>();
            internal::find_indices_and_values<double>(static_cast<double *>(inpptr), static_cast<double *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward);
            break;
        }
    }

    inline void find_subscripts(Mat input, Mat indsub, int forward)
    {
        void *inpptr;
        int *indsubptr = indsub.ptr<int>();

        switch (input.depth())
        {
        case CV_8U:
            inpptr = (void *)input.ptr<uchar>();
            internal::find_subscripts<uchar>(static_cast<uchar *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_8S:
            inpptr = (void *)input.ptr<char>();
            internal::find_subscripts<char>(static_cast<char *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_16U:
            inpptr = (void *)input.ptr<ushort>();
            internal::find_subscripts<ushort>(static_cast<ushort *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_16S:
            inpptr = (void *)input.ptr<short>();
            internal::find_subscripts<short>(static_cast<short *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_32S:
            inpptr = (void *)input.ptr<int>();
            internal::find_subscripts<int>(static_cast<int *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_32F:
            inpptr = (void *)input.ptr<float>();
            internal::find_subscripts<float>(static_cast<float *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_64F:
            inpptr = (void *)input.ptr<double>();
            internal::find_subscripts<double>(static_cast<double *>(inpptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        }
    }

    inline void find_subscripts_and_values(Mat input, Mat indsub, Mat val, int forward)
    {
        void *inpptr, *valptr;
        int *indsubptr = indsub.ptr<int>();

        switch (input.depth())
        {
        case CV_8U:
            inpptr = (void *)input.ptr<uchar>();
            valptr = (void *)val.ptr<uchar>();
            internal::find_subscripts_and_values<uchar>(static_cast<uchar *>(inpptr), static_cast<uchar *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_8S:
            inpptr = (void *)input.ptr<char>();
            valptr = (void *)val.ptr<char>();
            internal::find_subscripts_and_values<char>(static_cast<char *>(inpptr), static_cast<char *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_16U:
            inpptr = (void *)input.ptr<ushort>();
            valptr = (void *)val.ptr<ushort>();
            internal::find_subscripts_and_values<ushort>(static_cast<ushort *>(inpptr), static_cast<ushort *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_16S:
            inpptr = (void *)input.ptr<short>();
            valptr = (void *)val.ptr<short>();
            internal::find_subscripts_and_values<short>(static_cast<short *>(inpptr), static_cast<short *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_32S:
            inpptr = (void *)input.ptr<int>();
            valptr = (void *)val.ptr<int>();
            internal::find_subscripts_and_values<int>(static_cast<int *>(inpptr), static_cast<int *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_32F:
            inpptr = (void *)input.ptr<float>();
            valptr = (void *)val.ptr<float>();
            internal::find_subscripts_and_values<float>(static_cast<float *>(inpptr), static_cast<float *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        case CV_64F:
            inpptr = (void *)input.ptr<double>();
            valptr = (void *)val.ptr<double>();
            internal::find_subscripts_and_values<double>(static_cast<double *>(inpptr), static_cast<double *>(valptr), indsubptr, input.rows * input.cols * input.channels(), indsub.rows, forward, input.cols, input.channels());
            break;
        }
    }
}

pair<Mat, Mat> cvutil::find(Mat input, FindType type, int n, const string& direction)
{
    if (input.empty())
        return pair<Mat, Mat> (Mat(), Mat());

    int nchannels = input.channels();
    Mat sinput;
    int N, nOutputCols, forward = -1;

    // The countNonZero offered by OpenCV is hardware accelerated. 
    // Hence, it is better to use this function than reimplementing
    // it. But this function takes Mat objects having single channel.
    // So, we first reshape the input matrix and get a Mat header for
    // single channel image. Then call this function.
    if (nchannels > 1)
        sinput = input.reshape(1, input.rows);
    else
        sinput = input;
    
    // Now to call the countNonZero()
    N = countNonZero(sinput);

    // set configuration based on the arguments.
    if (N == 0)
        return pair<Mat, Mat>(Mat(), Mat());
    else if (n > 0)
    {
        CV_ASSERT2((direction == "first" || direction == "last"), "direction must be either \"first\" or \"last\".");

        if (direction == "first")
            forward = 1;
        else
            forward = 0;

        if (N >= n)
            N = n;
    }
    else
        forward = 1;
    
    switch (type)
    {
    case cvutil::FindType::Indices:
        nOutputCols = 1;
        break;
    case cvutil::FindType::IndicesAndValues:
        nOutputCols = 1;
        break;
    case cvutil::FindType::Subscripts:
        if (nchannels > 1)
            nOutputCols = 3;
        else
            nOutputCols = 2;
        break;
    case cvutil::FindType::SubscriptsAndValues:
        if (nchannels > 1)
            nOutputCols = 3;
        else
            nOutputCols = 2;
        break;
    default:
        break;
    }

    // Initialize the result matrix.
    Mat val{}; 
    Mat indsub = Mat::zeros(N, nOutputCols, CV_32SC1);
    int *indsubptr = indsub.ptr<int>();
    
    if (type == FindType::IndicesAndValues || type == FindType::SubscriptsAndValues)
        val = Mat::zeros(N, 1, input.type());

    switch (type)
    {
    case cvutil::FindType::Indices:
        FindHelper::find_indices(input, indsub, forward);
        break;
    case cvutil::FindType::IndicesAndValues:
        FindHelper::find_indices_and_values(input, indsub, val, forward); 
        break;
    case cvutil::FindType::Subscripts:
        FindHelper::find_subscripts(input, indsub, forward);
        break;
    case cvutil::FindType::SubscriptsAndValues:
        FindHelper::find_subscripts_and_values(input, indsub, val, forward); 
        break;
    default:
        return pair<Mat, Mat>(Mat(), Mat());
    }
    
    return pair<Mat, Mat>(indsub, val);
}

//Mat cvutil::bwdist(Mat input)
//{
//    Mat result;
//    distanceTransform(input, result, CV_DIST_L2, DIST_MASK_PRECISE, CV_32F);
//    return result;
//}

//Mat cvutil::bwdist2(Mat input, Mat& label, int labeltype)
//{
//    Mat result;
//    distanceTransform(input, result, label, CV_DIST_L2, DIST_MASK_PRECISE, labeltype);
//    return result;
//}

Mat cvutil::bwdist(Mat input)
{
    CV_ASSERT2(input.channels() == 1 && input.depth() == 0, "input must be single channel with depth CV_8U containing values 0 and 255 (binary image).");

    Mat out;

    if(checkHardwareSupport(CPU_AVX2))
        out = bwdist_helper::bwdist_mt(input.clone());
    else
        out = bwdist_helper::bwdist_st_no_avx(input.clone());

    //Mat result = out.rowRange(1, inputc.rows - 1).colRange(1, inputc.cols - 1);
    return out;
}

Mat cvutil::bwdist(Mat input, Mat& label, int labeltype)
{
    Mat result;
    distanceTransform(input, result, label, DIST_L2, DIST_MASK_PRECISE, labeltype);
    return result;
}