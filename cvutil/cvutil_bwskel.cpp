/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_bwskel.cpp

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

// bwskel implementation

#include "cvutil.h"
#include "cvutil_bwskel.h"

#pragma warning(disable : 4752)

//#define DIST(x)     (round(distptr[(x)] * 1000.0) / 1000.0)

using namespace std;
using namespace cv;

// Similar to a linked list, but it is 2-dimensional. Also, 
// it doesn't allocate/free memory. Only operates on already 
// allocated continuous memory.

/* 
* Class: location_base
* Similar to a linked list, but it is 2-dimensional. Also, 
* it doesn't allocate/free memory. Only operates on already 
* allocated continuous memory.
*  
* This class can also be thought of as a spaceship operator. An object
* of this class hovers on a pixel in a given binary image and retreives 
* the pixel information with respect to its neighbors. This class does
* not consider boundary conditions. Hence, we need to pass a binary image
* with extra boundaries.
*/

#if defined (__linux__)
static inline void *_aligned_malloc(size_t size, size_t alignment)
{
    // aligned_alloc requires size to be a multiple of alignment
    size_t aligned_size = ((size + alignment - 1) / alignment) * alignment;
    return aligned_alloc(alignment, aligned_size);
}

static inline void _aligned_free(void *ptr)
{
    free(ptr);
}

#endif

class location_base
{
    const unsigned int vecsize = 256;
    const unsigned int alignment = 32;

    void init_vectors()
    {
        // // We want these vaiables to be 32-byte aligned, as the alignment
        // // cannot be enforced if they are not pointers in the class.
        // // This problem may be resolved in the newer standard of C++ (c++17).
        // tn = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tn2 = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tp = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tp2 = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tele = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tzero = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // tpt = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment)); // -
        // PC2 = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment));
        // PC3 = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment));
        // PC4 = static_cast<__m256i *>(_aligned_malloc(vecsize, alignment));

        // PT = static_cast<__m256 *>(_aligned_malloc(vecsize, alignment));
        // PN = static_cast<__m256 *>(_aligned_malloc(vecsize, alignment));
        // PC = static_cast<__m256 *>(_aligned_malloc(vecsize, alignment));
    }

    void delete_vectors()
    {
        // _aligned_free(tn);
        // _aligned_free(tn2);
        // _aligned_free(tp);
        // _aligned_free(tp2);
        // _aligned_free(tele);
        // _aligned_free(tzero);
        // _aligned_free(tpt);
        // _aligned_free(PC2);
        // _aligned_free(PC3);
        // _aligned_free(PC4);

        // _aligned_free(PT);
        // _aligned_free(PN);
        // _aligned_free(PC);
    }
protected:
    int N[8], N2[8];
    alignas(32) __m256i tn, tn2, tp, tp2, tele, tzero, tpt, PC2, PC3, PC4;
    alignas(32) __m256 PT, PN, PC;

    // Single channel, CV_8U image matrix
    uchar* imgp = nullptr;
    bool avx2;

    int prevpt, pt;
    int nelements;
    int rows, cols;

public:
    location_base(Mat *_img, int _pt = 0) : imgp(_img->ptr<uchar>()), pt(_pt), prevpt(0),
        nelements(_img->rows * _img->cols), rows(_img->rows), cols(_img->cols)
    {
        int _N[8] = { 1, -cols + 1, -cols, -cols - 1, -1, cols - 1, cols, cols + 1 };
        int _N2[8] = { 2, -2 * cols + 2, -2 * cols, -2 * cols - 2, -2, 2 * cols - 2, 2 * cols, 2 * cols + 2 };

        for (int i = 0; i < 8; i++)
        {
            N[i] = _N[i];
            N2[i] = _N2[i];
        }

        avx2 = checkHardwareSupport(CPU_AVX2);

        if (avx2)
        {
            init_vectors();

            tn = _mm256_loadu_si256((__m256i const *)N);
            tn2 = _mm256_loadu_si256((__m256i const *)N2);

            tele = _mm256_set1_epi32(nelements);
            tzero = _mm256_set1_epi32(0);
        }
    }

    location_base(uchar *_img, int _rows, int _cols, int _pt = 0) : imgp(_img), pt(_pt), prevpt(0),
        nelements(_rows * _cols), rows(_rows), cols(_cols)
    {
        int _N[8] = { 1, -cols + 1, -cols, -cols - 1, -1, cols - 1, cols, cols + 1 };
        int _N2[8] = { 2, -2 * cols + 2, -2 * cols, -2 * cols - 2, -2, 2 * cols - 2, 2 * cols, 2 * cols + 2 };

        for (int i = 0; i < 8; i++)
        {
            N[i] = _N[i];
            N2[i] = _N2[i];
        }

        avx2 = checkHardwareSupport(CPU_AVX2);

        if (avx2)
        {
            init_vectors();

            tn = _mm256_loadu_si256((__m256i const *)N);
            tn2 = _mm256_loadu_si256((__m256i const *)N2);

            tele = _mm256_set1_epi32(nelements);
            tzero = _mm256_set1_epi32(0);
        }
    }
    
    ~location_base()
    {
        if (avx2)
            delete_vectors();
    }

    // We never perform a deep copy. We just copy the pointers and the
    // current position pt.
    location_base *clone()
    {
        location_base *result = new location_base(imgp, rows, cols, pt);
        return result;
    }

    void setpt(int _pt) { prevpt = pt; pt = _pt; }
    int getpt() { return pt; }

    uchar imgval() { return imgp[pt]; }
    uchar imgval(int _n) { return imgp[pt + N[_n]]; }
    void setimagepixel() { imgp[pt] = 255; }
    void unsetimagepixel() { imgp[pt] = 0; }

    int nval(int _n) { return N[_n]; }

    location_base *right() { prevpt = pt; pt += N[0]; return this; }
    location_base *rightup() { prevpt = pt; pt += N[1]; return this; }
    location_base *up() { prevpt = pt; pt += N[2]; return this; }
    location_base *leftup() { prevpt = pt; pt += N[3]; return this; }
    location_base *left() { prevpt = pt; pt += N[4]; return this; }
    location_base *leftdown() { prevpt = pt; pt += N[5]; return this; }
    location_base *down() { prevpt = pt; pt += N[6]; return this; }
    location_base *rightdown() { prevpt = pt; pt += N[7]; return this; }

    location_base *forward() // To be used only after one of the above functions is called
    {
        int p = prevpt;
        prevpt = pt;
        pt += (pt - p);

        return this;
    }

    location_base *backward() // To be used only after one of the above functions is called
    {
        int p = prevpt;
        prevpt = pt;
        pt -= (pt - p);

        return this;
    }

    location_base *goto_pt(int n)
    {
        // FIXME : Validate n in range [0, 7].
        prevpt = pt;
        pt += N[n];
        return this;
    }

    int getimageneighborsum()
    {
        int ncount = 0;
        for (int i = 0; i < 8; i++)
            if (imgp[pt + N[i]] == 255)
                ncount++;
        return ncount;
    }

    int getimageconnectivity()
    {
        int t1 = 0, t3 = 0, conn8 = 0, conn4 = 0, i1 = 0, i2 = 0, i3 = 0;
        int t2 = t1 + 4;

        // testing if the pixel preserves 8-connectivity
        for (t1 = 0, conn8 = 0; t1 < 8; t1 += 2)
        {
            t2 = (t1 + 1);
            t3 = (t1 + 2) % 8;

            if (imgp[pt + N[t1]] == 0 && (imgp[pt + N[t2]] != 0 || imgp[pt + N[t3]] != 0))
                conn8++;
        }

        // testing if the pixel preserves 4-connectivity
        for (t1 = 0, conn4 = 0; t1 < 8; t1 += 2)
        {
            t2 = (t1 + 1);
            t3 = (t1 + 2) % 8;

            i1 = imgp[pt + N[t1]] / 255;
            i2 = imgp[pt + N[t2]] / 255;
            i3 = imgp[pt + N[t3]] / 255;

            conn4 += (i1 - i1 * i2 * i3);
        }

        return max(conn8, conn4);
    }

    bool iscommonneighbor(int pt2, int n)
    {
        int nval = N[n];

        for (int nn = 0; nn < 8; nn++)
            if (pt + N[n] == pt2 + N[nn])
                return true;

        return false;
    }

    int xh8()
    {
        int t1, t2, t3, conn;

        for (t1 = 0, conn = 0; t1 < 8; t1 += 2)
        {
            t2 = (t1 + 1);
            t3 = (t1 + 2) % 8;

            if (imgp[pt + N[t1]] == 0 && (imgp[pt + N[t2]] != 0 || imgp[pt + N[t3]] != 0))
                conn++;
        }

        return conn;
    }
};

class location : public location_base
{
    //uchar *imgp = nullptr;
    float *distp = nullptr;
    uchar *ridgep = nullptr;
    int *labp = nullptr;

public:
    location(Mat *_img, Mat *_dist, Mat *_ridge, int _pt = 0) : location_base(_img, _pt), distp(_dist->ptr<float>()),
        ridgep(_ridge->ptr<uchar>()) {}
    location(uchar* _img, float *_dist, uchar *_ridge, int _rows, int _cols, int _pt = 0) : location_base(_img, _rows, _cols, _pt), distp(_dist),
        ridgep(_ridge) {}

    /*static location* create(Mat *_img, Mat *_dist, Mat *_ridge, int _pt = 0)
    {
        return new location(_img, _dist, _ridge, _pt);
    }*/

    // We never perform a deep copy. We just copy the pointers and the
    // current position pt.
    location *clone()
    {
        location *result = new location(imgp, distp, ridgep, rows, cols, pt);

        if (labp != nullptr)
            result->setlab(labp);

        return result;
    }

    void setlab(Mat _lab) { labp = _lab.ptr<int>(); }
    void setlab(int *_lab) { labp = _lab; }

    float distval() { return distp[pt]; }
    float distval(int _n) { return distp[pt + N[_n]]; }

    uchar ridgeval() { return ridgep[pt]; }
    uchar ridgeval(int _n) { return ridgep[pt + N[_n]]; }

    location *right() { prevpt = pt; pt += N[0]; return this; }
    location *rightup() { prevpt = pt; pt += N[1]; return this; }
    location *up() { prevpt = pt; pt += N[2]; return this; }
    location *leftup() { prevpt = pt; pt += N[3]; return this; }
    location *left() { prevpt = pt; pt += N[4]; return this; }
    location *leftdown() { prevpt = pt; pt += N[5]; return this; }
    location *down() { prevpt = pt; pt += N[6]; return this; }
    location *rightdown() { prevpt = pt; pt += N[7]; return this; }

    location *forward() // To be used only after one of the above functions is called
    {
        int p = prevpt;
        prevpt = pt;
        pt += (pt - p);

        return this;
    }

    location *backward() // To be used only after one of the above functions is called
    {
        int p = prevpt;
        prevpt = pt;
        pt -= (pt - p);

        return this;
    }

    location *goto_pt(int n)
    {
        // FIXME : Validate n in range [0, 7].
        prevpt = pt;
        pt += N[n];
        return this;
    }

    bool isridge()
    {
        int t1 = 0, t3 = 0, conn = 0, i1 = 0, i2 = 0, i3 = 0;
        int t2 = t1 + 4;
        float P = distp[pt];
        alignas(32) int indices[8];
        bool error_found = false;
        
        if (avx2)
        {
            tpt = _mm256_set1_epi32(pt);
            tp = _mm256_add_epi32(tpt, tn);

            PT = _mm256_set1_ps(distp[pt]);
            // PN = _MM_FUNC_I32(_mm256_set_ps, distp, tp);
            PN = _mm256_i32gather_ps(distp, tp, 4);

            PC = _mm256_cmp_ps(PT, PN, _CMP_GT_OQ);

            PC2 = _mm256_castps_si256(PC);
            PC3 = _mm256_permute2f128_si256(PC2, PC2, 0x81);
            PC4 = _mm256_and_si256(PC2, PC3);

            if (_mm256_movemask_epi8(PC4) > 0)
                return true;

            PC3 = _mm256_permute2f128_si256(PC2, PC2, 0x1);
            PC = _mm256_cmp_ps(PT, PN, _CMP_EQ_OQ);
            PC2 = _mm256_castps_si256(PC);
            PC2 = _mm256_and_si256(PC2, PC3);
            tp2 = _mm256_add_epi32(tpt, tn2);
            PC3 = _mm256_cmpgt_epi32(tele, tp2);
            PC2 = _mm256_and_si256(PC2, PC3);
            PC3 = _mm256_or_si256(_mm256_cmpgt_epi32(tp2, tzero), _mm256_cmpeq_epi32(tp2, tzero));
            PC2 = _mm256_and_si256(PC2, PC3);

            // for (t1 = 0; t1 < 8; t1++)
            //     if (M2U32(PC2, t1) && distp[M2I32(tp2, t1)] < P)
            //         return true;
            // Gather 8 float values from distp at offsets in (*tp2)
            PC = _mm256_i32gather_ps(distp, tp2, 4);
            PC = _mm256_cmp_ps(PC, PT, _CMP_LT_OQ);
            PC3 = _mm256_castps_si256(PC);
            PC2 = _mm256_and_si256(PC2, PC3);
            
            // Convert to mask
            if (((uint32_t)_mm256_movemask_epi8(PC2)) > 0)
                return true;
            
            // Gather 8 uchar values from imgp at offsets in (*tp)
            // (*PC2) = _MM_FUNC_I32(_mm256_set_epi32, imgp, (*tp));

            // We cannot use _mm256_i32gather_si256 here as it requires 32-bit integers
            // but the imgp is of type uchar.
            _mm256_store_si256((__m256i*)indices, tp);
            PC2 = _mm256_set_epi32(
                imgp[indices[7]], imgp[indices[6]], imgp[indices[5]], imgp[indices[4]],
                imgp[indices[3]], imgp[indices[2]], imgp[indices[1]], imgp[indices[0]]);
            
            if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(PC2, tzero)) == 0xffffffffU)
                return false;
        }
        else
        {
            for (t1 = 0, t2 = 4; t1 < 4; t1++, t2++)
            {
                if (distp[pt + N[t1]] < P && distp[pt + N[t2]] < P)
                    return true;
            }

            for (t1 = 0, t2 = 4; t1 < 8; t1++, t2 = (t2 + 1) % 8)
            {
                if (distp[pt + N[t1]] == P && distp[pt + N[t2]] < P &&
                    ((pt + N2[t1]) < nelements) && ((pt + N2[t1]) >= 0) &&
                    (distp[pt + N2[t1]] < P))
                    return true;
            }

            for (t1 = 0, conn = 0; t1 < 8; t1++)
                if (imgp[pt + N[t1]] != 0)
                    conn++;

            if (conn == 8)
                return false;
        }

        // testing if the pixel preserves 8-connectivity
        for (t1 = 0, conn = 0; t1 < 8; t1 += 2)
        {
            t2 = (t1 + 1);
            t3 = (t1 + 2) % 8;

            if (imgp[pt + N[t1]] == 0 && (imgp[pt + N[t2]] != 0 || imgp[pt + N[t3]] != 0))
                conn++;
        }

        if (conn > 1)
            return true;

        // testing if the pixel preserves 4-connectivity
        for (t1 = 0, conn = 0; t1 < 8; t1 += 2)
        {
            t2 = (t1 + 1);
            t3 = (t1 + 2) % 8;

            i1 = imgp[pt + N[t1]] / 255;
            i2 = imgp[pt + N[t2]] / 255;
            i3 = imgp[pt + N[t3]] / 255;

            conn += (i1 - i1 * i2 * i3);
        }

        if (conn > 1)
            return true;

        return false;
    }

    bool isridgeendpt()
    {
        int ncount = 0, ncount2 = 0;

        for (int i = 0; i < 8; i++)
            if (ridgep[pt + N[i]] == 255)
                ncount++;

        switch (ncount)
        {
        case 0:
        case 1:
            return true;
            break;
        case 2:
            for (int i = 0; i < 8; i++)
                if (ridgep[pt + N[i]] == 255 && ridgep[pt + N[(i + 1) % 8]] == 255)
                    ncount2++;

            if (ncount2 == 1)
                return true;
            return false;
            break;
        case 3:
            for (int i = 0; i < 8; i++)
                if (ridgep[pt + N[i]] == 255 && 
                    ridgep[pt + N[(i + 1) % 8]] == 255 && 
                    ridgep[pt + N[(i + 2) % 8]] == 255)
                    ncount2++;

            if (ncount2 == 1)
                return true;
            return false;
            break;
        default:
            return false;
            break;
        }
    }

    int getridgeneighborsum()
    {
        int ncount = 0;
        for (int i = 0; i < 8; i++)
            if (ridgep[pt + N[i]] == 255)
                ncount++;
        return ncount;
    }

    bool isisolatedneighbor(int n, bool checklab = true)
    {
        int t1 = (n + 1) % 8, t2 = (n + 7) % 8, t3 = (n + 2) % 8, t4 = (n + 6) % 8;
        int nc = 0;

        if (ridgep[pt + N[n]] != 255 && imgp[pt + N[n]] == 255 &&
            ridgep[pt + N[t1]] != 255 && ridgep[pt + N[t2]] != 255 && distp[pt + N[n]] >= distp[pt])
        {
            if (((n % 2) == 1) || ((n % 2) == 0 && ridgep[pt + N[t3]] != 255 && ridgep[pt + N[t4]] != 255))
            {
                if (checklab)
                {
                    for (int nn : N)
                    {
                        if (ridgep[pt + N[n] + nn] == 255 && ((N[n] + nn) != 0) && labp[pt] == labp[pt + N[n] + nn])
                        {
                            nc = 1;
                            break;
                        }
                    }

                    if (nc == 1)
                        return false;
                }
            }
            else
                return false;
        }
        else
            return false;

        return true;
    }
};

Mat bwskel_helper::bwskel(Mat inputc, Mat dist)
{
    pair<Mat, Mat> nzpixels = find(dist, FindType::Indices);
    Mat result = Mat::zeros(dist.size(), inputc.type());

    Mat ilab(result.size(), CV_32S);
    Mat lab(result.size(), CV_32S);
    int *labptr = lab.ptr<int>();
    int *ilabptr = ilab.ptr<int>();
    uchar *resultptr = result.ptr<uchar>();
    uchar *inputptr = inputc.ptr<uchar>();
    int *nzpixelsptr = nzpixels.first.ptr<int>();
    float *distptr = dist.ptr<float>();
    int npixels = nzpixels.first.rows;
    float P;
    vector<float> V = { 0,0,0,0,0,0,0,0 };
    float maxv;
    int nc;
    int loc, maxvcount, pt, p1, p2, p3, p4, newp1, newp2, newp3, newp4, nelements = dist.cols * dist.rows - 1;
    bool IsComplete;

    int nstart = 0;
    int nend = npixels;
    int thpt = 0;

    location *ploc = new location(&inputc, &dist, &result);
    
    // Ridge detection
    for (int i = nstart; i < nend; i++)
    {
        thpt = nzpixelsptr[i];
        ploc->setpt(thpt);

        if (ploc->isridge())
        {
            resultptr[thpt] = 255;
        }
    }

    //toc();
    //system("pause");
    
    // The ridge, so detected, is disconnected even though we passed
    // a single connected component. The disconnected end points may 
    // be a neighbor to one or two ridge points of the same connected 
    // component. Once we identify these end points on the disconnected
    // ridges, we can perform steepest ascent on the distance map to
    // connect them to the final skeleton.

    // First to find the connected components on ridge map.
    unordered_map<int, int> endptdict;
    endptdict.reserve(10000);

    auto ridgecomps = getConnectedComponents(result, lab);
    size_t prevcomps = 0, prevendpts = 0, cendpts = 0;
    int incomps = connectedComponents(inputc, ilab, 8, CV_32S);
    vector<int> maxcompsize(incomps);
    labptr = lab.ptr<int>();
    ploc->setlab(lab);
#define ROWIDX(x)  ((x) / dist.cols)
#define COLIDX(x)  ((x) % dist.cols)

    int nitr = 0;
    
    do
    {
        //window("wervv", inputc - result / 2);
        nitr++;
        
        // To find the end-points in these components that are inside the
        // distance map.
        vector<vector<int>> ridgeendpts;
        vector<bool> isemptyridge;
        ridgeendpts.resize(ridgecomps.size());
        isemptyridge.resize(ridgecomps.size());
        float nrv1 = -1.0f, nrv2 = -1.0f, Rsum;
        bool nearstate = false, isendpt = false;
        int neighborsum = 0;
        
        for (int i = 0; i < ridgecomps.size(); i++)
        {
            for (int pt : ridgecomps[i])
            {
                if (endptdict[pt] == 1)
                    continue;

                ploc->setpt(pt);
                isendpt = ploc->isridgeendpt();
                neighborsum = ploc->getridgeneighborsum();

                // To check and skip if the point is not an end point.
                if (isendpt || neighborsum == 2)
                {
                    P = ploc->distval();
                    nrv1 = -1.0f;
                    nrv2 = -1.0f;

                    for (int n = 0, t1 = 0, t2 = 0; n < 8; n++)
                    {
                        t1 = (n + 1) % 8;
                        t2 = (n + 7) % 8; // Cyclic for n - 1
                        //temp = ((n % 2) == 0) ? 1.0f : sqrtf(2.0f);

                        if (ploc->ridgeval(n) == 255)
                        {
                            if (nrv1 < (ploc->distval(n)))
                                nrv1 = (ploc->distval(n));
                        }
                        else
                        {
                            //if (ploc->isridgeendpt() || ploc->isisolatedneighbor(n))
                            //{
                                if (ploc->ridgeval(t1) == 0 &&
                                    ploc->ridgeval(t2) == 0 &&
                                    nrv2 < (ploc->distval(n)))
                                    nrv2 = (ploc->distval(n));
                            //}
                        }
                    }
                }
                else
                {
                    //endptdict[pt] = 1;
                    continue;
                }

                if (!((P >= nrv1) && (P <= nrv2)))
                {
                    //endptdict[pt] = 1;
                    continue;
                }

                if (isendpt)
                {
                    // Check for (4 and 8)-connectivity
                    nc = ploc->getimageconnectivity();
                    
                    // To check if the point is interior point
                    if (ploc->getimageneighborsum() >= 5 || (neighborsum == 0) ||
                        ((nc > 1) && neighborsum >= 1 && neighborsum <= 3))
                    {
                        ridgeendpts[i].push_back(pt);
                        isemptyridge[i] = false;
                    }
                }
                else if (neighborsum == 2)
                {
                    /*if (!((P >= nrv1) && (P <= nrv2)))
                        continue;*/

                    ridgeendpts[i].push_back(pt);
                    isemptyridge[i] = false;
                }
            }
        }
        
        for (int i = 0; i < ridgecomps.size(); i++)
        {
            if (maxcompsize[ilabptr[ridgecomps[i][0]]] < ridgecomps[i].size())
                maxcompsize[ilabptr[ridgecomps[i][0]]] = int(ridgecomps[i].size());
        }

        for (int i = 0; i < ridgecomps.size(); i++)
        {
            if (ridgeendpts[i].size() == 0 && ridgecomps[i].size() != maxcompsize[ilabptr[ridgecomps[i][0]]])
            {
                for (int pt : ridgecomps[i])
                {
                    //if (endptdict[pt] == 1)
                    //    continue;

                    ploc->setpt(pt);

                    // To check and skip if the point is not an end point.
                    Rsum = ploc->getridgeneighborsum();

                    if (!(Rsum == 2 || (Rsum == 3 && ploc->xh8() == 2))) // || (Rsum == 3) || (Rsum == 4)))
                        continue;

                    vector<float> vals, dists, newvals, newdists;
                    vector<int> locs, locsum, prevlocsum, locsorig, newlocs, newlocsum, prevnewlocsum, newlocsorig;
                    unordered_map<int, int> usedlocsum;
                    vals.resize(8);
                    locs.resize(8);
                    locsum.resize(8);
                    prevlocsum.resize(8);
                    locsorig.resize(8);
                    dists.resize(8);
                    int ncount = 0, locv = -1, found = 0, origloc;

                    maxvcount = 0;
                    for (int n = 0; n < 8; n++)
                    {
                        if (ploc->isisolatedneighbor(n))
                        {
                            vals[maxvcount] = ploc->ridgeval(n);
                            locsum[maxvcount] = ploc->nval(n);
                            usedlocsum[ploc->nval(n)] = 1;
                            prevlocsum[maxvcount] = 0;
                            locsorig[maxvcount] = ploc->nval(n);
                            locs[maxvcount] = n;
                            dists[maxvcount] = (n % 2 == 0) ? 1.0f : sqrtf(2.0f);
                            maxvcount++;
                        }
                    }

                    if (maxvcount == 0)
                        continue;

                    do
                    {
                        maxv = -10.0f;
                        ncount = 0;
                        locv = -1;
                        origloc = -1;

                        newvals.clear();
                        prevnewlocsum.clear();
                        newlocsum.clear();
                        newlocsorig.clear();
                        newdists.clear();
                        newlocs.clear();

                        for (int location = 0, t1 = 0, t2 = 0, nval = 0; location < maxvcount; location++)
                        {
                            t1 = (locs[location] + 1) % 8;
                            t2 = (locs[location] + 7) % 8; // Cyclic for location - 1

                            found = 0;
                            vector<int> locpts;

                            if (locs[location] % 2 == 0)
                            {
                                locpts = { locs[location] , (locs[location] + 1) % 8, (locs[location] + 7) % 8 };
                            }
                            else
                                locpts = { locs[location] , (locs[location] + 1) % 8, (locs[location] + 7) % 8, (locs[location] + 2) % 8, (locs[location] + 6) % 8 };

                            for (int locpt : locpts)
                            {
                                ploc->setpt(pt + locsum[location]);
                                nval = ploc->nval(locpt);

                                if (ploc->imgval(locpt) == 255)
                                {
                                    if (usedlocsum[locsum[location] + nval] == 0 && ((ploc->ridgeval(locpt) != 255) ||
                                        (ploc->ridgeval(locpt) == 255 &&
                                            labptr[pt + locsum[location] + nval] != labptr[ridgecomps[i][0]])))
                                    {
                                        newvals.push_back(ploc->ridgeval(locpt));
                                        prevnewlocsum.push_back(locsum[location]);
                                        newlocsum.push_back(locsum[location] + nval);
                                        usedlocsum[locsum[location] + nval] = 1;
                                        newlocsorig.push_back(locsorig[location]);
                                        newlocs.push_back(locpt);
                                        newdists.push_back(dists[location] + ((locpt % 2 == 0) ? 1.0f : sqrtf(2.0f)));
                                    }
                                }
                            }
                        }

                        vals = newvals;
                        prevlocsum = prevnewlocsum;
                        locsum = newlocsum;
                        locsorig = newlocsorig;
                        dists = newdists;
                        locs = newlocs;
                        maxvcount = int(vals.size());

                        for (int location = 0; location < maxvcount; location++)
                        {
                            if (maxv < (vals[location] / dists[location]))
                            {
                                maxv = (vals[location] / dists[location]);
                                ncount = 1;
                                locv = location;
                                origloc = locsorig[location];
                            }
                            else if (maxv == (vals[location] / dists[location]) && locv != location && origloc != locsorig[location])
                                ncount++;
                        }
                    } while (maxv <= 0 && maxvcount > 0);

                    if (locv >= 0 && maxv > 0)
                    {
                        if (maxv > 2.5f)
                        {
                            resultptr[pt + locsorig[locv]] = 255;
                            labptr[pt + locsorig[locv]] = labptr[pt];
                            ridgeendpts[i].push_back(pt + locsorig[locv]);
                            isemptyridge[i] = true;
                        }
                        else
                        {
                            resultptr[pt + locsorig[locv]] = 255;
                            labptr[pt + locsorig[locv]] = labptr[pt];
                        }
                    }
                }
            }
        }
        
        prevendpts = cendpts;
        cendpts = 0;
        for (int i = 0; i < ridgecomps.size(); i++)
        {
            cendpts += ridgeendpts[i].size();
        }

        if ((ridgecomps.size() == 0) || (ridgecomps.size() == 1 && ridgeendpts[0].size() == 0) || (prevcomps == ridgecomps.size() && prevendpts == cendpts))
            break;

        // For each inner end point, perform steepest ascent until a ridge is
        // found.
        for (int i = 0; i < ridgeendpts.size(); i++)
        {
            if (ridgecomps[i].size() == 0)
                continue;
            else // if (ridgecomps[i].size() == 1)
            {
                for (int ptloc : ridgeendpts[i])
                {
                    pt = ptloc;
                    ploc->setpt(pt);
                    
                    /*if ((ROWIDX(pt) == ROWIDX(pointpt) && COLIDX(pt) == COLIDX(pointpt)))
                        cout << "FOUND row = " << ROWIDX(pt) << " - col = " << COLIDX(pt) << " - idx = " << pt << "\n";*/

                    // When steepest ascent is run on end-points for each component,
                    // some ridge end-points in other components may become 
                    // connected points in the process. Hence, we need to
                    // test the necesary and sufficient condition that the 
                    // current point (pt) is an end-point before we proceed.
                    Rsum = ploc->getridgeneighborsum();

                    // To check and skip if the point is not an end point.
                    if (!((ploc->isridgeendpt() || (Rsum >= 2 && Rsum <= 4 && isemptyridge[i])) || (Rsum == 2)))
                        continue;
                    
                    // A ridge end-point may have 1 or 2 neighboring ridge points.
                    // In the steepest ascent, the current ridge point(s) is stored 
                    // as previous ridge point(s) for the next iteration. This is 
                    // to make sure that if we find another ridge-point, we ca stop 
                    // the steepest ascent.
                    p1 = INT_MIN;
                    p2 = INT_MIN;
                    p3 = INT_MIN;
                    p4 = INT_MIN;
                    newp1 = INT_MIN;
                    newp2 = INT_MIN;
                    newp3 = INT_MIN;
                    newp4 = INT_MIN;
                    IsComplete = false;

                    int startrow = pt / dist.cols;
                    int startcol = pt % dist.cols;
                    // cout << "Start i = " << i;

                    do
                    {
                        // Check if the ridge point is found. If running the
                        // first itertion, we need to update the p1, p2 and p3.
                        // For a ridge end-point the neighboring ridge points 
                        // are always adjacent.
                        if (p1 == INT_MIN && p2 == INT_MIN && p3 == INT_MIN && p4 == INT_MIN)
                        {
                            for (int n = 0, nval = 0; n < 8; n++)
                            {
                                nval = ploc->nval(n);

                                if (ploc->ridgeval(n) == 255)
                                {
                                    if (p1 == INT_MIN)
                                        p1 = nval;
                                    else if (p2 == INT_MIN && labptr[pt + nval] == labptr[pt + p1])
                                        p2 = nval;
                                    else if (p3 == INT_MIN && labptr[pt + nval] == labptr[pt + p1])
                                        p3 = nval;
                                    else if (labptr[pt + nval] == labptr[pt + p1])
                                        p4 = nval;
                                }
                            }
                        }
                        else
                        {
                            // To test if the algorithm is completed.
                            maxv = -1.0;
                            vector<float> pts;
                            vector<int> ptl;

                            for (int n = 0, nval = 0; n < 8; n++)
                            {
                                nval = ploc->nval(n);

                                if (ploc->ridgeval(n) == 255 && nval != p1 && nval != p2 && nval != p3 && nval != p4)
                                {
                                    if (maxv < ploc->distval(n))
                                        maxv = ploc->distval(n);
                                }
                            }

                            if (maxv > 0)
                            {
                                for (int n = 0, nval = 0; n < 8; n++)
                                {
                                    nval = ploc->nval(n);

                                    if (ploc->isisolatedneighbor(n, false) && ploc->distval(n) >= maxv &&
                                        ROWIDX(pt + nval) > 0 && ROWIDX(pt + nval) < dist.rows && 
                                        COLIDX(pt + nval) > 0 && COLIDX(pt + nval) < dist.cols)
                                    {
                                        pts.push_back(ploc->distval(n));
                                        ptl.push_back(n);
                                    }
                                }

                                if (pts.size() > 1)
                                {
                                    maxv = -1.0;
                                    loc = INT_MIN;

                                    for (int nn = 0; nn < pts.size(); nn++)
                                    {
                                        if (maxv < pts[nn])
                                        {
                                            maxv = pts[nn];
                                            loc = ptl[nn];
                                        }
                                    }

                                    maxv = 0;
                                    ploc->setpt(pt + ploc->nval(loc));

                                    for (int nn = 0; nn < 8; nn++)
                                        maxv += (ploc->ridgeval(nn) > 0 && !ploc->iscommonneighbor(pt, nn));

                                    ploc->setpt(pt);
                                    if (maxv > 1)
                                    {
                                        IsComplete = true;
                                        endptdict[pt] = 1;
                                        break;
                                    }
                                    else if (maxv == 1)
                                    {
                                        resultptr[pt + ploc->nval(loc)] = 255;
                                    }
                                }

                                endptdict[pt] = 1;
                                IsComplete = true;
                                break;
                            }
                        }

                        if (IsComplete)
                            break;

                        for (int n = 0; n < 8; n++)
                            V[n] = -10.0f;

                        for (int n = 0, nval = 0, t1 = 0, t2 = 0; n < 8; n++)
                        {
                            t1 = (n + 1) % 8;
                            t2 = (n + 7) % 8; // Cyclic for n - 1
                            nval = ploc->nval(n);

                            if (nval == p1 || nval == p2 || nval == p3 || nval == p4)
                            {
                                V[n] = -1.0f;
                                V[t1] = -1.0f;
                                V[t2] = -1.0f;
                            }
                        }

                        P = ploc->distval();
                        for (int n = 0; n < 8; n++)
                            if (V[n] != -1.0f && ploc->imgval(n) > 0)
                                V[n] = ploc->distval(n);

                        // To find max distance and its location
                        maxv = -10000;
                        loc = INT_MIN;
                        maxvcount = 0;

                        for (int n = 0; n < 8; n++)
                        {
                            if (maxv < V[n])
                            {
                                maxv = V[n];
                                loc = ploc->nval(n);
                                maxvcount = 1;
                            }
                            else if (maxv == V[n])
                                maxvcount++;
                        }

                        // If the neighboring pixels have only one pixel with max value,
                        // we update the next point directly. But if we find more than one point
                        // we need to do voting.
                        if (maxvcount > 1)
                        {
                            // We now look into the next layer of the neighborhood values
                            // for a better selection.
                            vector<float> vals, dists;
                            vector<int> locs, locsum, locsorig;
                            vals.resize(maxvcount);
                            locs.resize(maxvcount);
                            locsum.resize(maxvcount);
                            locsorig.resize(maxvcount);
                            dists.resize(maxvcount);
                            int ncount = 0, locv = -1, origloc;

                            for (int n = 0, location = 0, nval = 0; n < 8; n++)
                            {
                                nval = ploc->nval(n);

                                if (maxv == V[n])
                                {
                                    vals[location] = ploc->distval(n);
                                    locsum[location] = nval;
                                    locsorig[location] = nval;
                                    locs[location] = n;
                                    dists[location] = (n % 2 == 0) ? 1.0f : sqrtf(2.0f);
                                    location++;
                                }
                            }

                            maxv = -10.0f;
                            for (int location = 0; location < maxvcount; location++)
                            {
                                if (maxv < (vals[location] / dists[location]))
                                {
                                    maxv = (vals[location]) / dists[location];
                                    ncount = 1;
                                    locv = location;
                                    origloc = locsorig[location];
                                }
                                else if (maxv == (vals[location] / dists[location]) && locv != location && origloc != locsorig[location])
                                    ncount++;
                            }

                            if(ncount == 1)
                                loc = locsorig[locv];

                            while (ncount != 1)
                            {
                                maxv = -10.0f;
                                ncount = 0;
                                locv = -1;
                                origloc = -1;

                                for (int location = 0, t1 = 0, t2 = 0; location < maxvcount; location++)
                                {
                                    ploc->setpt(pt + locsum[location]);

                                    t1 = (locs[location] + 1) % 8;
                                    t2 = (locs[location] + 7) % 8;

                                    if (ploc->imgval(t1) == 255 && ploc->imgval(t2) == 255)
                                    {
                                        if (ploc->distval(locs[location]) >= ploc->distval(t1) &&
                                            (ploc->distval(locs[location]) >= ploc->distval(t2)))
                                        {
                                            vals[location] += ploc->distval(locs[location]);
                                            locsum[location] += ploc->nval(locs[location]);
                                            dists[location] += (locs[location] % 2 == 0) ? 1.0f : sqrtf(2.0f);
                                        }
                                        else if (ploc->distval(t1) >= ploc->distval(t2))
                                        {
                                            vals[location] += ploc->distval(t1);
                                            locsum[location] += ploc->nval(t1);
                                            locs[location] = t1;
                                            dists[location] += (t1 % 2 == 0) ? 1.0f : sqrtf(2.0f);
                                        }
                                        else
                                        {
                                            vals[location] += ploc->distval(t2);
                                            locsum[location] += ploc->nval(t2);
                                            locs[location] = t2;
                                            dists[location] += (t2 % 2 == 0) ? 1.0f : sqrtf(2.0f);
                                        }
                                    }
                                }

                                for (int location = 0; location < maxvcount; location++)
                                {
                                    if (maxv < (vals[location] / dists[location]))
                                    {
                                        maxv = (vals[location]) / dists[location];
                                        ncount = 1;
                                        locv = location;
                                        origloc = locsorig[location];
                                    }
                                    else if (maxv == (vals[location] / dists[location]) && locv != location && origloc != locsorig[location])
                                        ncount++;
                                }
                            }

                            loc = locsorig[locv];
                        }

                        if (p1 != INT_MIN)
                            for (int n = 0; n < 8; n++)
                                if (pt + p1 == pt + loc + ploc->nval(n))
                                    newp1 = ploc->nval(n);
                        if (p2 != INT_MIN)
                            for (int n = 0; n < 8; n++)
                                if (pt + p2 == pt + loc + ploc->nval(n))
                                    newp2 = ploc->nval(n);
                        if (p3 != INT_MIN)
                            for (int n = 0; n < 8; n++)
                                if (pt + p3 == pt + loc + ploc->nval(n))
                                    newp3 = ploc->nval(n);
                        if (p4 != INT_MIN)
                            for (int n = 0; n < 8; n++)
                                if (pt + p4 == pt + loc + ploc->nval(n))
                                    newp4 = ploc->nval(n);

                        if (newp3 == INT_MIN)
                        {
                            newp3 = newp4;
                            newp4 = INT_MIN;
                        }
                        if (newp2 == INT_MIN)
                        {
                            newp2 = newp3;
                            newp3 = INT_MIN;
                        }
                        if (newp1 == INT_MIN)
                        {
                            newp1 = newp2;
                            newp2 = INT_MIN;
                        }
                        if (newp2 == INT_MIN)
                        {
                            newp2 = newp3;
                            newp3 = INT_MIN;
                        }
                        if (newp3 == INT_MIN)
                        {
                            newp3 = newp4;
                            newp4 = INT_MIN;
                        }

                        p1 = -loc;
                        p2 = newp1;
                        p3 = newp2;
                        p4 = newp3;
                        pt = pt + loc;
                        resultptr[pt] = 255;
                        ploc->setpt(pt);
                        newp1 = INT_MIN;
                        newp2 = INT_MIN;
                        newp3 = INT_MIN;
                        newp4 = INT_MIN;
                    } while (true);
                }
            }
        }
        
        prevcomps = ridgecomps.size();
        ridgecomps = getConnectedComponents(result, lab);
        //connectedComponents(result, lab, 8, CV_32S);
        labptr = lab.ptr<int>();
        ploc->setlab(lab);
    } while (true); // || nitr <= 1);
    
    // To remove 1-pixel holes in ridges 
    if (ridgecomps.size() > 0)
    {
        for (int pt : ridgecomps[0])
        {
            ploc->setpt(pt);

            for (int n = 0, t1 = 0, t2 = 0, nval = 0; n < 8; n += 2)
            {
                t1 = (n + 1) % 8;
                t2 = (n + 7) % 8;
                nval = ploc->nval(n);

                if (ploc->ridgeval(n) == 0 && ploc->ridgeval(t1) > 0 && ploc->ridgeval(t2) > 0)
                    if (resultptr[pt + 2 * nval] > 0)
                        resultptr[pt + nval] = 255;
            }
        }

        delete ploc;

        Mat result2 = bwthin(result);
        return result2;
    }
    else
    {
        delete ploc;
        return result;
    }
    //window("eewewee", result2);
    //Mat result3 = result2.rowRange(1, dist.rows - 1).colRange(1, dist.cols - 1);

    /*vector<Mat> ch;
    Mat result4 = bwthin(input);
    Mat newi = input - (result3 + result4);

    ch.push_back(newi);
    ch.push_back(newi + result4);
    ch.push_back(newi + result3);

    Mat out;
    merge(ch, out);

    return out;*/
}

void bwskel_helper::rectify_components(Mat& skeleton, Mat dist, Mat inputc)
{
    int ncols = skeleton.cols;
    float *dptr = dist.ptr<float>();
    uchar *iptr = inputc.ptr<uchar>();
    int pt = 0;

    location_base* ploc = new location_base(&skeleton);
    ploc->setpt(2 * ncols);

    // Operate on first row only and correct the components
    // The first 2 rows are padded with zeros so we start on 3rd row.
    while (ploc->getpt() < (3 * ncols))
    {
        while (ploc->imgval() == 0 && ploc->getpt() < (3 * ncols))
            ploc = ploc->right();

        switch (ploc->getimageneighborsum())
        {
        case 0:
        case 1:
            if (dptr[ploc->getpt() + 1] > dptr[ploc->getpt()] && 
                iptr[ploc->getpt()] > 0 && ploc->getpt() < (3 * ncols) && 
                ploc->getimageneighborsum() < 2 && ploc->getimageconnectivity() < 2)
            {
                do
                {
                    ploc->unsetimagepixel();
                    ploc = ploc->right();
                } while (dptr[ploc->getpt() + 1] > dptr[ploc->getpt()] && 
                         iptr[ploc->getpt()] > 0 && ploc->getpt() < (3 * ncols) && 
                         ploc->getimageneighborsum() < 2 && ploc->getimageconnectivity() < 2);
            }
            else
                ploc = ploc->right();
            break;
        default:
            ploc = ploc->right();
            break;
        }
    }

    while (ploc->getpt() >= (2 * ncols))
    {
        pt = ploc->getpt();

        while (ploc->imgval() == 0 && ploc->getpt() >= (2 * ncols))
            ploc = ploc->left();

        pt = ploc->getimageneighborsum();
        switch (ploc->getimageneighborsum())
        {
        case 0:
        case 1:
            if (dptr[ploc->getpt() - 1] > dptr[ploc->getpt()] && 
                iptr[ploc->getpt()] > 0 && ploc->getpt() >= (2 * ncols) && 
                ploc->getimageneighborsum() <= 2 && ploc->getimageconnectivity() < 2)
            {
                do
                {
                    ploc->unsetimagepixel();
                    ploc = ploc->left();
                } while (dptr[ploc->getpt() - 1] > dptr[ploc->getpt()] && 
                         iptr[ploc->getpt()] > 0 && ploc->getpt() >= (2 * ncols) && 
                         ploc->getimageneighborsum() <= 2 && ploc->getimageconnectivity() < 2);
            }
            else
                ploc = ploc->left();
            break;
        default:
            ploc = ploc->left();
            break;
        }
    }

    delete ploc;
}


