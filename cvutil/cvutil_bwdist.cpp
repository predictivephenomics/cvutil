/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_bwdist.cpp

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

// bwdist Implementation
//
// Source:
// Felzenszwalb PF, Huttenlocher DP (2012) Distance transforms of sampled functions. 
// Theory of computing, Vol 8 (1):415-428

#include "cvutil_bwdist.h"

#include "cvutil.h"
#include "cvutil_matlab_interface.h"
#include "cvutil_templates.h"

#include <thread>

#pragma warning(disable : 4752)

using namespace cv;

// Reference implementation.
Mat bwdist_helper::bwdist_st_no_avx(Mat m)
{
    Mat result = (m != 0);
    result.convertTo(result, CV_32FC1);
    result /= 255.0;
    result *= FLT_MAX;
    float *resultptr = result.ptr<float>();
    int ncol = m.cols, nrow = m.rows, nelements = m.cols * m.rows, k, i, j;
    Mat vmat, zmat, arrmat, dmat;
    int *v, *a;
    float *z, *d;
    float s = 0;
    
    vmat = Mat::zeros(1, max(ncol, nrow), CV_32SC1);
    v = vmat.ptr<int>();
    zmat = Mat::ones(1, max(ncol, nrow) + 1, CV_32FC1);
    z = zmat.ptr<float>();
    dmat = Mat::zeros(1, nrow, CV_32FC1);
    d = dmat.ptr<float>();
    arrmat = Mat::ones(1, nrow, CV_32SC1);
    a = arrmat.ptr<int>();

    for (i = 0; i < nrow; i++)
        a[i] = i * ncol;
    
    // Horizontal scan
    for (i = 0; i < nrow; i++)
    {
        v[0] = 0;
        z[0] = -FLT_MAX; z[1] = FLT_MAX;
        k = 0;
        
        for (j = 1; j < ncol; j++)
        {
            s = (resultptr[a[i] + j] - resultptr[a[i] + v[k]] +
                (j * j - v[k] * v[k])) / float(2 * (j - v[k]));

            while (s <= z[k])
            {
                k--;
                s = (resultptr[a[i] + j] - resultptr[a[i] + v[k]] +
                    (j * j - v[k] * v[k])) / float(2 * (j - v[k]));
            }

            k++;
            v[k] = j;
            z[k] = s;
            z[k + 1] = FLT_MAX;
        }

        for (j = 0, k = 0; j < ncol; j++)
        {
            while (z[k + 1] < j)
                k++;

            resultptr[a[i] + j] = (j - v[k]) * (j - v[k]) + resultptr[a[i] + v[k]];
        }
    }

    // Vertical scan
    for (j = 0; j < ncol; j++)
    {
        v[0] = 0;
        z[0] = -FLT_MAX; z[1] = FLT_MAX;
        k = 0;

        for (i = 1; i < nrow; i++)
        {
            s = (resultptr[a[i] + j] - resultptr[a[v[k]] + j] +
                (i * i - v[k] * v[k])) / float(2 * (i - v[k]));
            
            while (s <= z[k])
            {
                k--;
                s = (resultptr[a[i] + j] - resultptr[a[v[k]] + j] +
                    (i * i - v[k] * v[k])) / float(2 * (i - v[k]));
            }

            k++;
            v[k] = i;
            z[k] = s;
            z[k + 1] = FLT_MAX;
        }

        for (i = 0, k = 0; i < nrow; i++)
        {
            while (z[k + 1] < i)
                k++;

            d[i] = ((i - v[k]) * (i - v[k]) + resultptr[a[v[k]] + j]);
        }

        for (i = 0, k = 0; i < nrow; i++)
            resultptr[a[i] + j] = d[i];
    }

    for (i = 0; i < nelements; i++)
    {
        resultptr[i] = (resultptr[i] >= 2) ? sqrtf(resultptr[i]) : resultptr[i];
    }

    return result;
}

void horizontal_st_avx(Mat& result, Mat& arrmat, int tid = -1)
{
    int ncol = result.cols, nrow = result.rows, nelements = result.cols * result.rows, k, i = 0, j, p;
    int *v, *a;
    float *z;
    float s = 0;
    int stepsize = 8, mask;
    int nstart, nend, nthreads = getNumberOfCPUs();

    Mat vmat, zmat;

    vmat = Mat::zeros(8, max(ncol, nrow), CV_32SC1);
    zmat = Mat::ones(8, max(ncol, nrow) + 1, CV_32FC1);

    float *resultptr = result.ptr<float>();
    v = vmat.ptr<int>();
    z = zmat.ptr<float>();
    a = arrmat.ptr<int>();

    // AVX specific
    __m256i tbuf1, tbuf2, vk, vofkidx, vofk, vj, tzero, tone, vstep, va, vi;
    __m256 fbuf, fbuf1, fbuf2, fbuf3, fbuf4, nflt, flt, fzero, zofk;

    // Thread management
    if (tid == -1)
    {
        nstart = 0;
        nend = nrow;
    }
    else
    {
        nstart = tid * nrow / nthreads;
        nend = (tid == (nthreads - 1))? (nrow) : ((tid + 1) * nrow / nthreads);
    }

    ////////////// For row indexing //////////////
    // Check for memory alignment before performing vector operations.
    tbuf1 = _mm256_set_epi32((7 + nstart) * ncol, (6 + nstart) * ncol, (5 + nstart) * ncol, (4 + nstart) * ncol, (3 + nstart) * ncol, (2 + nstart) * ncol, (1 + nstart) * ncol, nstart * ncol);
    tbuf2 = _mm256_set1_epi32(stepsize * ncol);

    for (i = nstart; i < nend; i += stepsize)
    {
        // Vector acceleration for memory.
        if ((i + stepsize) <= nend)
        {
            _mm256_storeu_si256((__m256i *)&a[i], tbuf1);
            tbuf1 = _mm256_add_epi32(tbuf1, tbuf2);
        }
        else // Normal code for the last unaligned elements.
            for (p = 0; p < (nend - i + 1); p++)
                a[i + p] = (i + p) * ncol;
    }

    //////////////////// Horizontal scan ////////////////////
    fzero = _mm256_set1_ps(0);
    tbuf1 = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
    fbuf2 = _mm256_set_ps(8, 7, 6, 5, 4, 3, 2, 1);
    nflt = _mm256_set1_ps(-FLT_MAX);
    flt = _mm256_set1_ps(FLT_MAX);
    vstep = _mm256_set1_epi32(stepsize);
    tzero = _mm256_set1_epi32(0);
    tone = _mm256_set1_epi32(1);

    // vj varies per iteration and may need to be re-initialized in vertical scan.
    vj = _mm256_set_epi32(7 + nstart, 6 + nstart, 5 + nstart, 4 + nstart, 3 + nstart, 2 + nstart, 1 + nstart, 0 + nstart);
    vj = _mm256_sub_epi32(vj, vstep);

    for (i = nstart; i < nend; i += stepsize)
    {
        if ((i + stepsize) <= nend)
        {
            _mm256_storeu_si256((__m256i *)&v[0], tzero);
            _mm256_storeu_ps(&z[0], nflt);
            _mm256_storeu_ps(&z[stepsize], flt);
            vk = _mm256_set1_epi32(0);
            vj = _mm256_add_epi32(vj, vstep);

            vi = _mm256_set_epi32(a[vj.m256i_i32[7]], a[vj.m256i_i32[6]], a[vj.m256i_i32[5]], a[vj.m256i_i32[4]],
                a[vj.m256i_i32[3]], a[vj.m256i_i32[2]], a[vj.m256i_i32[1]], a[vj.m256i_i32[0]]);

            for (j = 1; j < ncol; j++)
            {
                va = _mm256_set1_epi32(j);
                va = _mm256_add_epi32(vi, va);

                // Get resultptr[a[i] + j]
                fbuf = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                    resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                // Get v[k]
                vofkidx = _mm256_mullo_epi32(vk, vstep);
                vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                    v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                // Get z[k]
                zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7]], z[vofkidx.m256i_i32[6]], z[vofkidx.m256i_i32[5]], z[vofkidx.m256i_i32[4]],
                    z[vofkidx.m256i_i32[3]], z[vofkidx.m256i_i32[2]], z[vofkidx.m256i_i32[1]], z[vofkidx.m256i_i32[0]]);

                // Get resultptr[a[i] + v[k]]
                va = _mm256_add_epi32(vi, vofk);
                fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                    resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                fbuf4 = _mm256_set1_ps(float(j));
                fbuf2 = _mm256_cvtepi32_ps(vofk);
                fbuf1 = _mm256_sub_ps(fbuf, fbuf1);             // resultptr[a[i] + j] - resultptr[a[i] + v[k]]
                fbuf3 = _mm256_sub_ps(fbuf4, fbuf2);            // (j - v[k])
                fbuf2 = _mm256_add_ps(fbuf4, fbuf2);            // (j + v[k])
                fbuf1 = _mm256_fmadd_ps(fbuf3, fbuf2, fbuf1);   // (resultptr[a[i] + j] - resultptr[a[i] + v[k]] + (j * j - v[k] * v[k]))
                fbuf3 = _mm256_add_ps(fbuf3, fbuf3);            // 2 * (j - v[k])

                fbuf3 = _mm256_div_ps(fbuf1, fbuf3);            // s value computed

                fbuf1 = _mm256_cmp_ps(fbuf3, zofk, _CMP_LE_OQ);
                mask = _mm256_movemask_ps(fbuf1);

                while (mask > 0)
                {
                    if (mask == 0xff)
                    {
                        vk = _mm256_sub_epi32(vk, tone);
                    }
                    else
                    {
                        for (int p = 0; p < 8; p++)
                            if (fbuf1.m256_f32[p] != 0)
                                vk.m256i_i32[p]--;
                    }

                    // Reconstruct v[k]
                    vofkidx = _mm256_mullo_epi32(vk, vstep);
                    vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                    vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                        v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                    // Reconstruct z[k]
                    zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7]], z[vofkidx.m256i_i32[6]], z[vofkidx.m256i_i32[5]], z[vofkidx.m256i_i32[4]],
                        z[vofkidx.m256i_i32[3]], z[vofkidx.m256i_i32[2]], z[vofkidx.m256i_i32[1]], z[vofkidx.m256i_i32[0]]);

                    // Get new resultptr[a[i] + v[k]]
                    va = _mm256_add_epi32(vi, vofk);
                    fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                        resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                    fbuf2 = _mm256_cvtepi32_ps(vofk);
                    fbuf1 = _mm256_sub_ps(fbuf, fbuf1);             // resultptr[a[i] + j] - resultptr[a[i] + v[k]]
                    fbuf3 = _mm256_sub_ps(fbuf4, fbuf2);            // (j - v[k])
                    fbuf2 = _mm256_add_ps(fbuf4, fbuf2);            // (j + v[k])
                    fbuf1 = _mm256_fmadd_ps(fbuf3, fbuf2, fbuf1);   // (resultptr[a[i] + j] - resultptr[a[i] + v[k]] + (j * j - v[k] * v[k]))
                    fbuf3 = _mm256_add_ps(fbuf3, fbuf3);            // 2 * (j - v[k])

                    fbuf3 = _mm256_div_ps(fbuf1, fbuf3);            // s value updated

                    fbuf1 = _mm256_cmp_ps(fbuf3, zofk, _CMP_LE_OQ);
                    mask = _mm256_movemask_ps(fbuf1);
                }

                vk = _mm256_add_epi32(vk, tone);
                vofkidx = _mm256_mullo_epi32(vk, vstep);
                vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                for (int p = 0; p < 8; p++)
                {
                    v[vofkidx.m256i_i32[p]] = j;
                    z[vofkidx.m256i_i32[p]] = fbuf3.m256_f32[p];
                    z[vofkidx.m256i_i32[p] + stepsize] = FLT_MAX;
                }
            }

            // Second loop to assign the distance transform values
            vk = _mm256_set1_epi32(0);
            vofkidx = _mm256_mullo_epi32(vk, vstep);
            vofkidx = _mm256_add_epi32(vofkidx, tbuf1);
            zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7] + stepsize], z[vofkidx.m256i_i32[6] + stepsize], z[vofkidx.m256i_i32[5] + stepsize], z[vofkidx.m256i_i32[4] + stepsize],
                z[vofkidx.m256i_i32[3] + stepsize], z[vofkidx.m256i_i32[2] + stepsize], z[vofkidx.m256i_i32[1] + stepsize], z[vofkidx.m256i_i32[0] + stepsize]);

            for (j = 0; j < ncol; j++)
            {
                fbuf4 = _mm256_set1_ps(float(j));
                fbuf1 = _mm256_cmp_ps(zofk, fbuf4, _CMP_LT_OQ);
                mask = _mm256_movemask_ps(fbuf1);

                while (mask > 0)
                {
                    if (mask == 0xff)
                    {
                        vk = _mm256_add_epi32(vk, tone);
                    }
                    else
                    {
                        for (int p = 0; p < 8; p++)
                            if (fbuf1.m256_f32[p] != 0)
                                vk.m256i_i32[p]++;
                    }

                    vofkidx = _mm256_mullo_epi32(vk, vstep);
                    vofkidx = _mm256_add_epi32(vofkidx, tbuf1);
                    zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7] + stepsize], z[vofkidx.m256i_i32[6] + stepsize], z[vofkidx.m256i_i32[5] + stepsize], z[vofkidx.m256i_i32[4] + stepsize],
                        z[vofkidx.m256i_i32[3] + stepsize], z[vofkidx.m256i_i32[2] + stepsize], z[vofkidx.m256i_i32[1] + stepsize], z[vofkidx.m256i_i32[0] + stepsize]);
                    fbuf1 = _mm256_cmp_ps(zofk, fbuf4, _CMP_LT_OQ);
                    mask = _mm256_movemask_ps(fbuf1);
                }

                vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                    v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                // Get new resultptr[a[i] + v[k]]
                va = _mm256_add_epi32(vi, vofk);
                fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                    resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                fbuf2 = _mm256_cvtepi32_ps(vofk);
                fbuf4 = _mm256_sub_ps(fbuf4, fbuf2);            // Compute (j - v[k])
                fbuf1 = _mm256_fmadd_ps(fbuf4, fbuf4, fbuf1);   // Compute ((j - v[k]) * (j - v[k]) + resultptr[a[i] + v[k]])

                for (int p = 0; p < 8; p++)
                    resultptr[vi.m256i_i32[p] + j] = fbuf1.m256_f32[p];
            }
        }
        else
        {
            for (p = 0; p < (nend - i); p++)
            {
                v[0] = 0;
                z[0] = -FLT_MAX; z[1] = FLT_MAX;
                k = 0;

                for (j = 1; j < ncol; j++)
                {
                    s = (resultptr[a[i + p] + j] - resultptr[a[i + p] + v[k]] +
                        (j * j - v[k] * v[k])) / float(2 * (j - v[k]));

                    while (s <= z[k])
                    {
                        k--;
                        s = (resultptr[a[i + p] + j] - resultptr[a[i + p] + v[k]] +
                            (j * j - v[k] * v[k])) / float(2 * (j - v[k]));
                    }

                    k++;
                    v[k] = j;
                    z[k] = s;
                    z[k + 1] = FLT_MAX;
                }

                for (j = 0, k = 0; j < ncol; j++)
                {
                    while (z[k + 1] < j)
                        k++;

                    resultptr[a[i + p] + j] = (j - v[k]) * (j - v[k]) + resultptr[a[i + p] + v[k]];
                }
            }
        }
    }
}

void vertical_st_avx(Mat& result, Mat& arrmat, int tid = -1)
{
    int ncol = result.cols, nrow = result.rows, nelements = result.cols * result.rows, k, i = 0, j, p;
    int *v, *a;
    float *z, *d;
    float s = 0;
    int stepsize = 8, mask;
    int nstart, nend, nthreads = getNumberOfCPUs();

    Mat vmat, zmat, dmat;

    vmat = Mat::zeros(8, max(ncol, nrow), CV_32SC1);
    zmat = Mat::ones(8, max(ncol, nrow) + 1, CV_32FC1);
    dmat = Mat::zeros(8, nrow, CV_32FC1);

    float *resultptr = result.ptr<float>();
    v = vmat.ptr<int>();
    z = zmat.ptr<float>();
    d = dmat.ptr<float>();
    a = arrmat.ptr<int>();

    // AVX specific
    __m256i tbuf1, vk, vofkidx, vofk, vj, tzero, tone, vstep, va;
    __m256 fbuf, fbuf1, fbuf2, fbuf3, fbuf4, nflt, flt, fzero, zofk;

    // Thread management
    if (tid == -1)
    {
        nstart = 0;
        nend = ncol;
    }
    else
    {
        nstart = tid * ncol / nthreads;
        nend = (tid == (nthreads - 1)) ? (ncol): ((tid + 1) * ncol / nthreads);
    }

    fzero = _mm256_set1_ps(0);
    tbuf1 = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
    fbuf2 = _mm256_set_ps(8, 7, 6, 5, 4, 3, 2, 1);
    nflt = _mm256_set1_ps(-FLT_MAX);
    flt = _mm256_set1_ps(FLT_MAX);
    vstep = _mm256_set1_epi32(stepsize);
    tzero = _mm256_set1_epi32(0);
    tone = _mm256_set1_epi32(1);

    //////////////////// Vertical scan ////////////////////
    vj = _mm256_set_epi32(7 + nstart, 6 + nstart, 5 + nstart, 4 + nstart, 3 + nstart, 2 + nstart, 1 + nstart, 0 + nstart);
    vj = _mm256_sub_epi32(vj, vstep);

    for (j = nstart; j < nend; j += stepsize)
    {
        if ((j + stepsize) <= nend)
        {
            _mm256_storeu_si256((__m256i *)&v[0], tzero);
            _mm256_storeu_ps(&z[0], nflt);
            _mm256_storeu_ps(&z[stepsize], flt);
            vk = _mm256_set1_epi32(0);
            vj = _mm256_add_epi32(vj, vstep);

            for (i = 1; i < nrow; i++)
            {
                // Get resultptr[a[i] + j]
                fbuf = _mm256_loadu_ps(&resultptr[a[i] + j]);

                // Get v[k]
                vofkidx = _mm256_mullo_epi32(vk, vstep);
                vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                    v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                // Get z[k]
                zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7]], z[vofkidx.m256i_i32[6]], z[vofkidx.m256i_i32[5]], z[vofkidx.m256i_i32[4]],
                    z[vofkidx.m256i_i32[3]], z[vofkidx.m256i_i32[2]], z[vofkidx.m256i_i32[1]], z[vofkidx.m256i_i32[0]]);

                // Get resultptr[a[v[k]] + j]
                va = _mm256_set_epi32(a[vofk.m256i_i32[7]], a[vofk.m256i_i32[6]], a[vofk.m256i_i32[5]], a[vofk.m256i_i32[4]],
                    a[vofk.m256i_i32[3]], a[vofk.m256i_i32[2]], a[vofk.m256i_i32[1]], a[vofk.m256i_i32[0]]);
                va = _mm256_add_epi32(va, vj);
                fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                    resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                fbuf4 = _mm256_set1_ps(float(i));
                fbuf2 = _mm256_cvtepi32_ps(vofk);
                fbuf1 = _mm256_sub_ps(fbuf, fbuf1);             // resultptr[a[i] + j] - resultptr[a[v[k]] + j]
                fbuf3 = _mm256_sub_ps(fbuf4, fbuf2);            // (i - v[k])
                fbuf2 = _mm256_add_ps(fbuf4, fbuf2);            // (i + v[k])
                fbuf1 = _mm256_fmadd_ps(fbuf3, fbuf2, fbuf1);   // (resultptr[a[i] + j] - resultptr[a[v[k]] + j] + (i * i - v[k] * v[k]))
                fbuf3 = _mm256_add_ps(fbuf3, fbuf3);            // 2 * (i - v[k])

                fbuf3 = _mm256_div_ps(fbuf1, fbuf3);            // s value computed

                fbuf1 = _mm256_cmp_ps(fbuf3, zofk, _CMP_LE_OQ);
                mask = _mm256_movemask_ps(fbuf1);

                while (mask > 0)
                {
                    if (mask == 0xff)
                    {
                        vk = _mm256_sub_epi32(vk, tone);
                    }
                    else
                    {
                        for (int p = 0; p < 8; p++)
                            if (fbuf1.m256_f32[p] != 0)
                                vk.m256i_i32[p]--;
                    }

                    // Reconstruct v[k]
                    vofkidx = _mm256_mullo_epi32(vk, vstep);
                    vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                    vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                        v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                    // Reconstruct z[k]
                    zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7]], z[vofkidx.m256i_i32[6]], z[vofkidx.m256i_i32[5]], z[vofkidx.m256i_i32[4]],
                        z[vofkidx.m256i_i32[3]], z[vofkidx.m256i_i32[2]], z[vofkidx.m256i_i32[1]], z[vofkidx.m256i_i32[0]]);

                    // Get new resultptr[a[v[k]] + j]
                    va = _mm256_set_epi32(a[vofk.m256i_i32[7]], a[vofk.m256i_i32[6]], a[vofk.m256i_i32[5]], a[vofk.m256i_i32[4]],
                        a[vofk.m256i_i32[3]], a[vofk.m256i_i32[2]], a[vofk.m256i_i32[1]], a[vofk.m256i_i32[0]]);
                    va = _mm256_add_epi32(va, vj);
                    fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                        resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                    fbuf2 = _mm256_cvtepi32_ps(vofk);
                    fbuf1 = _mm256_sub_ps(fbuf, fbuf1);             // resultptr[a[i] + j] - resultptr[a[v[k]] + j]
                    fbuf3 = _mm256_sub_ps(fbuf4, fbuf2);            // (i - v[k])
                    fbuf2 = _mm256_add_ps(fbuf4, fbuf2);            // (i + v[k])
                    fbuf1 = _mm256_fmadd_ps(fbuf3, fbuf2, fbuf1);   // (resultptr[a[i] + j] - resultptr[a[v[k]] + j] + (i * i - v[k] * v[k]))
                    fbuf3 = _mm256_add_ps(fbuf3, fbuf3);            // 2 * (i - v[k])

                    fbuf3 = _mm256_div_ps(fbuf1, fbuf3);            // s value updated

                    fbuf1 = _mm256_cmp_ps(fbuf3, zofk, _CMP_LE_OQ);
                    mask = _mm256_movemask_ps(fbuf1);
                }

                /*for (int p = 0; p < 8; p++)
                vk.m256i_i32[p]++;*/
                vk = _mm256_add_epi32(vk, tone);
                vofkidx = _mm256_mullo_epi32(vk, vstep);
                vofkidx = _mm256_add_epi32(vofkidx, tbuf1);

                for (int p = 0; p < 8; p++)
                {
                    v[vofkidx.m256i_i32[p]] = i;
                    z[vofkidx.m256i_i32[p]] = fbuf3.m256_f32[p];
                    z[vofkidx.m256i_i32[p] + stepsize] = FLT_MAX;
                }
            }

            // Second loop to assign the distance transform values
            vk = _mm256_set1_epi32(0);
            vofkidx = _mm256_mullo_epi32(vk, vstep);
            vofkidx = _mm256_add_epi32(vofkidx, tbuf1);
            zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7] + stepsize], z[vofkidx.m256i_i32[6] + stepsize], z[vofkidx.m256i_i32[5] + stepsize], z[vofkidx.m256i_i32[4] + stepsize],
                z[vofkidx.m256i_i32[3] + stepsize], z[vofkidx.m256i_i32[2] + stepsize], z[vofkidx.m256i_i32[1] + stepsize], z[vofkidx.m256i_i32[0] + stepsize]);

            for (i = 0; i < nrow; i++)
            {
                fbuf4 = _mm256_set1_ps(float(i));
                fbuf1 = _mm256_cmp_ps(zofk, fbuf4, _CMP_LT_OQ);
                mask = _mm256_movemask_ps(fbuf1);

                while (mask > 0)
                {
                    if (mask == 0xff)
                    {
                        vk = _mm256_add_epi32(vk, tone);
                    }
                    else
                    {
                        for (int p = 0; p < 8; p++)
                            if (fbuf1.m256_f32[p] != 0)
                                vk.m256i_i32[p]++;
                    }

                    vofkidx = _mm256_mullo_epi32(vk, vstep);
                    vofkidx = _mm256_add_epi32(vofkidx, tbuf1);
                    zofk = _mm256_set_ps(z[vofkidx.m256i_i32[7] + stepsize], z[vofkidx.m256i_i32[6] + stepsize], z[vofkidx.m256i_i32[5] + stepsize], z[vofkidx.m256i_i32[4] + stepsize],
                        z[vofkidx.m256i_i32[3] + stepsize], z[vofkidx.m256i_i32[2] + stepsize], z[vofkidx.m256i_i32[1] + stepsize], z[vofkidx.m256i_i32[0] + stepsize]);
                    fbuf1 = _mm256_cmp_ps(zofk, fbuf4, _CMP_LT_OQ);
                    mask = _mm256_movemask_ps(fbuf1);
                }

                vofk = _mm256_set_epi32(v[vofkidx.m256i_i32[7]], v[vofkidx.m256i_i32[6]], v[vofkidx.m256i_i32[5]], v[vofkidx.m256i_i32[4]],
                    v[vofkidx.m256i_i32[3]], v[vofkidx.m256i_i32[2]], v[vofkidx.m256i_i32[1]], v[vofkidx.m256i_i32[0]]);

                // Get new resultptr[a[v[k]] + j]
                va = _mm256_set_epi32(a[vofk.m256i_i32[7]], a[vofk.m256i_i32[6]], a[vofk.m256i_i32[5]], a[vofk.m256i_i32[4]],
                    a[vofk.m256i_i32[3]], a[vofk.m256i_i32[2]], a[vofk.m256i_i32[1]], a[vofk.m256i_i32[0]]);
                va = _mm256_add_epi32(va, vj);
                fbuf1 = _mm256_set_ps(resultptr[va.m256i_i32[7]], resultptr[va.m256i_i32[6]], resultptr[va.m256i_i32[5]], resultptr[va.m256i_i32[4]],
                    resultptr[va.m256i_i32[3]], resultptr[va.m256i_i32[2]], resultptr[va.m256i_i32[1]], resultptr[va.m256i_i32[0]]);

                fbuf2 = _mm256_cvtepi32_ps(vofk);
                fbuf4 = _mm256_sub_ps(fbuf4, fbuf2);            // Compute (i - v[k])
                fbuf1 = _mm256_fmadd_ps(fbuf4, fbuf4, fbuf1);   // Compute ((i - v[k]) * (i - v[k]) + resultptr[a[v[k]] + p + j])

                _mm256_storeu_ps(&d[i * stepsize], fbuf1);
            }

            fbuf1 = _mm256_set1_ps(2.0f);
            for (i = 0; i < nrow; i++)
            {
                fbuf = _mm256_loadu_ps(&d[i * stepsize]);
                fbuf2 = _mm256_cmp_ps(fbuf, fbuf1, _CMP_LT_OQ);
                mask = _mm256_movemask_ps(fbuf2);

                if (mask == 0xff)
                    _mm256_storeu_ps(&resultptr[a[i] + j], fbuf);
                else
                    _mm256_storeu_ps(&resultptr[a[i] + j], _mm256_sqrt_ps(fbuf));
            }
        }
        else
        {
            for (p = 0; p < (nend - j); p++)
            {
                v[0] = 0;
                z[0] = -FLT_MAX; z[1] = FLT_MAX;
                k = 0;

                for (i = 1; i < nrow; i++)
                {
                    s = (resultptr[a[i] + p + j] - resultptr[a[v[k]] + p + j] +
                        (i * i - v[k] * v[k])) / float(2 * (i - v[k]));
                    
                    while (s <= z[k])
                    {
                        k--;
                        s = (resultptr[a[i] + p + j] - resultptr[a[v[k]] + p + j] +
                            (i * i - v[k] * v[k])) / float(2 * (i - v[k]));
                    }

                    k++;
                    v[k] = i;
                    z[k] = s;
                    z[k + 1] = FLT_MAX;
                }

                for (i = 0, k = 0; i < nrow; i++)
                {
                    while (z[k + 1] < i)
                        k++;

                    d[i] = ((i - v[k]) * (i - v[k]) + resultptr[a[v[k]] + p + j]);
                }

                for (i = 0, k = 0; i < nrow; i++)
                    resultptr[a[i] + p + j] = (d[i] >= 2) ? sqrtf(d[i]) : d[i];
            }
        }
    }
}

Mat bwdist_helper::bwdist_st_avx(Mat m)
{
    Mat result = (m != 0);
    result.convertTo(result, CV_32FC1);
    result /= 255.0;
    result *= FLT_MAX;
    Mat vmat, zmat, arrmat, dmat;
    int ncol = result.cols, nrow = result.rows;

    vmat = Mat::zeros(8, max(ncol, nrow), CV_32SC1);
    zmat = Mat::ones(8, max(ncol, nrow) + 1, CV_32FC1);
    dmat = Mat::zeros(8, nrow, CV_32FC1);
    arrmat = Mat::ones(1, nrow, CV_32SC1);
    
    horizontal_st_avx(result, arrmat);
    vertical_st_avx(result, arrmat);

    return result;
}

Mat bwdist_helper::bwdist_mt(Mat m)
{
    Mat result = (m != 0);
    result.convertTo(result, CV_32FC1);
    result /= 255.0;
    result *= FLT_MAX;
    Mat arrmat;
    int ncol = result.cols, nrow = result.rows;
    int nthreads = getNumberOfCPUs();

    arrmat = Mat::ones(1, nrow, CV_32SC1);

    std::thread *threads = new std::thread[nthreads];

    for (int tid = 0; tid < nthreads; tid++)
        threads[tid] = std::thread(horizontal_st_avx, result, arrmat, tid);

    for (int tid = 0; tid < nthreads; tid++)
        threads[tid].join();

    for (int tid = 0; tid < nthreads; tid++)
        threads[tid] = std::thread(vertical_st_avx, result, arrmat, tid);

    for (int tid = 0; tid < nthreads; tid++)
        threads[tid].join();

    delete[] threads;

    return result;
}
