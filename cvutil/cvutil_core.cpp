/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: cvutil_core.cpp

Author: Anand Seethepalli (aseethepalli@noble.org)
Principal Investigator: Larry York (lmyork@noble.org)
Root Phenomics Lab
Noble Research Institute, LLC

This file is part of Computer Vision UTILity toolkit (cvutil)

cvutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cvutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cvutil.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cvutil.h"

#include "cvutil_bwthin.h"
#include "cvutil_linesim.h"
#include "cvutil_bwskel.h"

using namespace std;
using namespace cv;

vector<WinData *> GlobalValues::figures;
//bool GlobalValues::cswitch = false;

Mat cvutil::getSingleChannel(Mat m)
{
    Mat out;

    // Make sure we have a single channel image
    if (m.channels() == 3)
    {
        vector<Mat> ch;
        split(m, ch);
        out = ch[0].clone();
    }
    else
        out = m.clone();

    return out;
}

Mat cvutil::getSingleChannel(Mat m, int type)
{
    // Making sure 'type' refers to single channel...
    CV_ASSERT2(CV_MAT_CN(type) == 1, "type must refer to single channel image.");

    Mat out;

    out = getSingleChannel(m);
    
    if (out.type() != type)
        out.convertTo(out, type);

    return out;
}

// Performs edge detection on 3 channel color image
// using Canny edge detector
Mat cvutil::getEdgeDetect(Mat img, int lim)
{
    // Canny detector
    vector<Mat> ch;
    //int lim = 0;

    split(img, ch);
    Canny(ch[0], ch[0], lim, lim * 3, 3);
    Canny(ch[1], ch[1], lim, lim * 3, 3);
    Canny(ch[2], ch[2], lim, lim * 3, 3);

    Mat merged = ch[0] | ch[1] | ch[2];

    return merged;
}

Mat cvutil::getLargestConnectedComponent(Mat img)
{
    Mat out;
    size_t cc, maxcc = 0;
    int maxidx = -1;
    vector<vector<int>> components = getConnectedComponents(img);
    size_t ncomp = components.size();

    for (int i = 0; i < ncomp; i++)
    {
        cc = components[i].size();

        if (cc > maxcc)
        {
            maxcc = cc;
            maxidx = i;
        }
    }
    
    // Now to reconstruct the image
    if (maxidx == -1)
        return Mat::zeros(1, 1, CV_8UC1);

    Mat lab = Mat::zeros(img.size(), CV_8UC1);
    uchar* data = lab.ptr<uchar>();
    uchar c = static_cast<uchar>(255);

    for (int i = 0; i < components[maxidx].size(); i++)
        data[components[maxidx][i]] = c;

    out = lab;

    return out;
}

vector<vector<int>> cvutil::getConnectedComponents(Mat img, int size, int conn)
{
    CV_ASSERT2(img.channels() == 1, "img must be a single channel image.");
    CV_ASSERT2(conn == 4 || conn == 8, "connectivity value must be either 4 or 8.");
    int ncomp, npixels = 0, nrequiredcomp = 0, i;
    Mat m = img.clone();
    Mat lab(img.size(), CV_32S);
    m.convertTo(m, CV_8UC1);

    ncomp = connectedComponents(m, lab, conn);
    vector<vector<int>> result;
    
    int nelements = img.size().height * img.size().width;
    int *data = lab.ptr<int>();
    int *idx;
    int *rvals;
    int **vals;

    vector<int> nsizedata(ncomp);
    int *nsize = nsizedata.data();

    for (i = 0; i < nelements; i++)
    {
        if (data[i] != 0)
            nsize[data[i] - 1]++;
    }
    
    if (size <= 0)
    {
        ncomp--;
        result.resize(ncomp);
        idx = new int[ncomp]();
        vals = new int*[ncomp];

        for (i = 0; i < ncomp; i++)
        {
            result[i].resize(nsize[i]);
            vals[i] = result[i].data();
        }
        for (i = 0; i < nelements; i++)
        {
            if (data[i] != 0)
            {
                vals[data[i] - 1][idx[data[i] - 1]] = i;
                idx[data[i] - 1]++;
            }
        }
    }
    else
    {
        for (i = 0, nrequiredcomp = 0; i < ncomp; i++)
        {
            if (nsize[i] > size)
                nrequiredcomp++;
        }

        result.resize(nrequiredcomp);
        idx = new int[nrequiredcomp]();
        rvals = new int[ncomp]();
        vals = new int*[nrequiredcomp];

        for (i = 0, nrequiredcomp = 0; i < ncomp; i++)
        {
            if (nsize[i] > size)
            {
                result[nrequiredcomp].resize(nsize[i]);
                vals[nrequiredcomp] = result[nrequiredcomp].data();
                rvals[i] = nrequiredcomp;
                nrequiredcomp++;
            }
            else
                rvals[i] = -1;
        }

        for (i = 0; i < nelements; i++)
        {
            if (data[i] != 0 && rvals[data[i] - 1] >= 0)
            {
                vals[rvals[data[i] - 1]][idx[rvals[data[i] - 1]]] = i;
                idx[rvals[data[i] - 1]]++;
            }
        }

        delete[] rvals;
    }

    delete[] vals;
    delete[] idx;

    return result;
}

void onMouse(int event, int _x, int _y, int flags, void *data)
{
    if (event != CV_EVENT_LBUTTONUP && //event != CV_EVENT_MOUSEMOVE && 
        event != CV_EVENT_LBUTTONDOWN && event != CV_EVENT_MOUSEWHEEL)
        return;

    WinData *dt = static_cast<WinData *>(data);
    Mat m = (*dt).m;

    if (event == CV_EVENT_LBUTTONUP)
    {
        int x = _x + dt->xzoomshift, y = _y + dt->yzoomshift;
        if (m.depth() == CV_8U)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << (int)m.at<Vec3b>(y, x)[0] << ", " << (int)m.at<Vec3b>(y, x)[1] << ", " << (int)m.at<Vec3b>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << (int)m.at<uchar>(y, x) << "\n";
        }
        else if (m.depth() == CV_16U)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << m.at<Vec3w>(y, x)[0] << ", " << m.at<Vec3w>(y, x)[1] << ", " << m.at<Vec3w>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << m.at<ushort>(y, x) << "\n";
        }
        else if (m.depth() == CV_16S)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << m.at<Vec3s>(y, x)[0] << ", " << m.at<Vec3s>(y, x)[1] << ", " << m.at<Vec3s>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << m.at<short>(y, x) << "\n";
        }
        else if (m.depth() == CV_32S)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << m.at<Vec3i>(y, x)[0] << ", " << m.at<Vec3i>(y, x)[1] << ", " << m.at<Vec3i>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << m.at<int>(y, x) << "\n";
        }
        else if (m.depth() == CV_32F)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << m.at<Vec3f>(y, x)[0] << ", " << m.at<Vec3f>(y, x)[1] << ", " << m.at<Vec3f>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << m.at<float>(y, x) << "\n";
        }
        else if (m.depth() == CV_64F)
        {
            if (m.channels() == 3)
                cout << "row = " << y << "    col = " << x << "    value = (" << m.at<Vec3d>(y, x)[0] << ", " << m.at<Vec3d>(y, x)[1] << ", " << m.at<Vec3d>(y, x)[2] << ")\n";
            else
                cout << "row = " << y << "    col = " << x << "    idx = " << y * m.cols + x << "    value = " << m.at<double>(y, x) << "\n";
        }

        if (dt->panmode == true)
        {
            dt->lbuttondownx = -1;
            dt->lbuttondowny = -1;

            dt->panmode = false;
        }
    }
    else if (event == CV_EVENT_LBUTTONDOWN)
    {
        dt->lbuttondownx = _x;
        dt->lbuttondowny = _y;
    }
    else if (event == CV_EVENT_MOUSEWHEEL && dt->panmode == false)
    {
        int wheelmove = getMouseWheelDelta(flags) / 120;
        int percentinc = 2;
        int xsize, ysize, xmin, ymin, xmax, ymax;
        int x = _x + dt->xzoomshift, y = _y + dt->yzoomshift;
        
        if (wheelmove > 0 && dt->zoom > percentinc) // zoom-in
        {
            // Update final zoom
            if (dt->zoom < 10)
                dt->zoom = max(percentinc, dt->zoom - wheelmove * percentinc);
            else
                dt->zoom = max(percentinc, dt->zoom - wheelmove * percentinc * 3);

            xsize = (m.cols * dt->zoom) / 100;
            ysize = (m.rows * dt->zoom) / 100;

            xmin = (x - xsize / 2 < 0) ? 0 : x - xsize / 2 - ((x + xsize / 2 > m.cols) ? (x + xsize / 2 - m.cols) : 0);
            xmax = (x + xsize / 2 > m.cols) ? m.cols : ((x - xsize / 2 < 0) ? xsize : x + xsize / 2);

            ymin = (y - ysize / 2 < 0) ? 0 : y - ysize / 2 - ((y + ysize / 2 > m.rows) ? (y + ysize / 2 - m.rows) : 0);
            ymax = (y + ysize / 2 > m.rows) ? m.rows : ((y - ysize / 2 < 0) ? ysize : y + ysize / 2);

            imshow(dt->winname, m.rowRange(ymin, ymax).colRange(xmin, xmax));
            dt->yzoomshift = ymin;
            dt->xzoomshift = xmin;

            dt->xsize = xsize;
            dt->ysize = ysize;
        }
        else if (wheelmove < 0) // zoom-out
        {
            if (dt->zoom < 100)
            {
                // Update final zoom
                if (dt->zoom < 10)
                    dt->zoom = min(100, dt->zoom - wheelmove * percentinc);
                else
                    dt->zoom = min(100, dt->zoom - wheelmove * percentinc * 3);

                xsize = (m.cols * dt->zoom) / 100;
                ysize = (m.rows * dt->zoom) / 100;

                xmin = (x - xsize / 2 < 0) ? 0 : x - xsize / 2 - ((x + xsize / 2 > m.cols) ? (x + xsize / 2 - m.cols) : 0);
                xmax = (x + xsize / 2 > m.cols) ? m.cols : ((x - xsize / 2 < 0) ? xsize : x + xsize / 2);

                ymin = (y - ysize / 2 < 0) ? 0 : y - ysize / 2 - ((y + ysize / 2 > m.rows) ? (y + ysize / 2 - m.rows) : 0);
                ymax = (y + ysize / 2 > m.rows) ? m.rows : ((y - ysize / 2 < 0) ? ysize : y + ysize / 2);

                imshow(dt->winname, m.rowRange(ymin, ymax).colRange(xmin, xmax));
                dt->yzoomshift = ymin;
                dt->xzoomshift = xmin;

                dt->xsize = xsize;
                dt->ysize = ysize;
            }
        }
    }

    m.release();
}

void cvutil::printheader(Mat m)
{
    if (m.cols >= 4 && m.rows >= 4)
        cout << m.rowRange(0, 4).colRange(0, 4) << endl;
    else if (m.cols >= 4)
        cout << m.rowRange(0, m.rows).colRange(0, 4) << endl;
    else if (m.rows >= 4)
        cout << m.rowRange(0, 4).colRange(0, m.cols) << endl;
    else
        cout << m << endl;
}

// Similar to imshow() in opencv, but has 
//     * An onMouse event Handler.
//     * We do not have to call namedWindow() before calling
//       window().
//     * Supports zooming in and out of the image.
//     * Displaying the same image (having same array address)
//       with the same window name does not change the zoom
//       of the image.
//     * Clicking on any pixel will print its coordinates and
//       the pixel value on console.
void cvutil::window(cv::String winname, Mat m)
{
    bool iswdfound = false;
    WinData *wd = nullptr;

    // To check if a window exists already with this name 'winname'
    for (WinData *_wd : GlobalValues::figures)
    {
        if (_wd->winname == winname)
        {
            iswdfound = true;
            wd = _wd;
        }
    }

    if (iswdfound && wd->m.rows == m.rows && wd->m.cols == m.cols)
    {
        wd->m = m;
        imshow(winname, m.rowRange(wd->yzoomshift, wd->yzoomshift + wd->ysize).colRange(wd->xzoomshift, wd->xzoomshift + wd->xsize));
    }
    else
    {
        wd = new WinData();
        wd->m = m;
        wd->winname = winname;

        namedWindow(winname, WINDOW_NORMAL);
        wd->hWnd = (HWND)cvGetWindowHandle(winname.c_str());
        wd->xsize = m.cols;
        wd->ysize = m.rows;

        imshow(winname, m);
        GlobalValues::figures.push_back(wd);
    }

    setMouseCallback(winname, onMouse, static_cast<void*>(wd));
    waitKey(0);
}

Mat cvutil::bwthin(Mat input)
{
    CV_ASSERT2(input.channels() == 1 && input.depth() == 0, "input must be single channel with depth CV_8U containing values 0 and 255 (binary image).");

    Mat inputc = input.clone();
    
    // Add 1-pixel width of black pixels as boundary to the input image to avoid
    // boundary errors.
    hconcat(Mat::zeros(input.rows, 1, input.type()), inputc, inputc);
    hconcat(inputc, Mat::zeros(input.rows, 1, input.type()), inputc);

    vconcat(Mat::zeros(1, inputc.cols, input.type()), inputc, inputc);
    vconcat(inputc, Mat::zeros(1, inputc.cols, input.type()), inputc);

    pair<Mat, Mat> psubs = find(inputc == 255, FindType::Subscripts);
    Mat subs = psubs.first;
    Mat inds = sub2ind(inputc.size(), subs);
    hconcat(subs, Mat::ones(subs.rows, 2, subs.type()), subs);

    Mat out;

    if (useOptimized())
        out = bwthin_helper::bwthin_mt_thread(inputc, subs, inds);
    else
        out = bwthin_helper::bwthin_st(inputc, subs, inds);

    Mat result = out.rowRange(1, inputc.rows - 1).colRange(1, inputc.cols - 1);
    return result.clone();
}

Mat cvutil::bwskel(Mat input, Mat distance)
{
    CV_ASSERT2(input.channels() == 1 && input.depth() == 0, "input must be single channel with depth CV_8U containing values 0 and 255 (binary image).");
    CV_ASSERT2(distance.empty() || (distance.channels() == 1 && distance.depth() == CV_32F), "distance must be either empty or a single channel CV_32F matrix.");
    
    Mat inputc = input.clone();
    Mat distc;
    
    if (distance.empty())
        distc = bwdist(inputc);
    else
        distc = distance;
    
    // Round to 3 decimal places.
    //distc = floor(distc * 1000.0f + 0.5f) / 1000.0f;
    
    // Add 1-pixel width of black pixels as boundary to the input image to avoid
    // boundary errors.
    hconcat(Mat::zeros(distc.rows, 1, distc.type()), distc, distc);
    hconcat(distc, Mat::zeros(distc.rows, 1, distc.type()), distc);

    vconcat(Mat::zeros(1, distc.cols, distc.type()), distc, distc);
    vconcat(distc, Mat::zeros(1, distc.cols, distc.type()), distc);

    hconcat(Mat::zeros(inputc.rows, 1, inputc.type()), inputc, inputc);
    hconcat(inputc, Mat::zeros(inputc.rows, 1, inputc.type()), inputc);

    vconcat(Mat::zeros(1, inputc.cols, inputc.type()), inputc, inputc);
    vconcat(inputc, Mat::zeros(1, inputc.cols, inputc.type()), inputc);
    
    // To make Mats continuous
    Mat dist = distc.clone();
    inputc = inputc.clone();
    
    Mat out = bwskel_helper::bwskel(inputc, dist);
    //Mat result = out.rowRange(1, out.rows - 1).colRange(1, out.cols - 1);
    
    bwskel_helper::rectify_components(out, dist, inputc);
    
    return out.rowRange(1, out.rows - 1).colRange(1, out.cols - 1).clone();
}

Mat cvutil::linesim(Mat input, LineSimplificationType type, double epsilon)
{
    CV_ASSERT2(input.channels() == 1 && input.depth() == 0, "input must be single channel with depth CV_8U containing values 0 and 255 (binary image).");
    
    Mat inputc = input.clone();

    // Add 1-pixel width of black pixels as boundary to the input image to avoid
    // boundary errors.
    hconcat(Mat::zeros(input.rows, 1, input.type()), inputc, inputc);
    hconcat(inputc, Mat::zeros(input.rows, 1, input.type()), inputc);

    vconcat(Mat::zeros(1, inputc.cols, input.type()), inputc, inputc);
    vconcat(inputc, Mat::zeros(1, inputc.cols, input.type()), inputc);

    Mat out = linesim_helper::linesim_st(inputc, epsilon);
    
    Mat result = out.rowRange(1, inputc.rows - 1).colRange(1, inputc.cols - 1);
    return result;
}

Mat cvutil::getMat(InputArray y)
{
    Mat result;

    switch (y.kind())
    {
    case _InputArray::MAT:
    case _InputArray::MATX:
    case _InputArray::EXPR:
    case _InputArray::STD_VECTOR:
    case _InputArray::UMAT:
        result = y.getMat();

        if (y.kind() == _InputArray::STD_VECTOR)
        {
            if (result.rows == 1)
                result = result.t();
        }

        break;
    case _InputArray::STD_VECTOR_VECTOR:
    {
        vector<Mat> mats;
        y.getMatVector(mats);

        for (int i = 0; i < mats.size(); i++)
            mats[i] = mats[i].t();

        cv::hconcat(mats, result);
    }
    break;
    case _InputArray::STD_VECTOR_MAT:
    case _InputArray::STD_VECTOR_UMAT:
        // To concatenate the matrices horizontally
        cv::hconcat(y, result);
        break;
    }

    return result;
}

Mat cvutil::addBorder(Mat input, Scalar color, int thickness)
{
    Mat result;
    
    CV_ASSERT2((thickness > 0), "Thickness should be greater than zero.");
    CV_ASSERT2((input.channels() == 1 || input.channels() == 3), "The image should be a 3-channel color image or single channel grascale image.");

    if (input.channels() == 3)
    {
        vector<Mat> oldchannels, newchannels;
        split(input, oldchannels);

        for (int i = 0; i < 3; i++)
        {
            newchannels[i] = oldchannels[i].clone();
            hconcat(Mat::ones(oldchannels[i].rows, thickness, oldchannels[i].type()) * color[i], newchannels[i], newchannels[i]);
            hconcat(newchannels[i], Mat::ones(oldchannels[i].rows, thickness, oldchannels[i].type()) * color[i], newchannels[i]);

            vconcat(Mat::ones(thickness, newchannels[i].cols, oldchannels[i].type()) * color[i], newchannels[i], newchannels[i]);
            vconcat(newchannels[i], Mat::ones(thickness, newchannels[i].cols, oldchannels[i].type()) * color[i], newchannels[i]);
        }

        merge(newchannels, result);
    }
    else if (input.channels() == 1)
    {
        result = input.clone();
        hconcat(Mat::ones(input.rows, thickness, input.type()) * color[0], result, result);
        hconcat(result, Mat::ones(input.rows, thickness, input.type()) * color[0], result);

        vconcat(Mat::ones(thickness, result.cols, input.type()) * color[0], result, result);
        vconcat(result, Mat::ones(thickness, result.cols, input.type()) * color[0], result);
    }
    
    return result;
}

Mat cvutil::removeBorder(Mat input, int thickness)
{
    Mat result;

    CV_ASSERT2((thickness > 0), "Thickness should be greater than zero.");
    CV_ASSERT2((input.channels() == 1 || input.channels() == 3), "The image should be a 3-channel color image or single channel grascale image.");

    if (input.channels() == 3)
    {
        vector<Mat> oldchannels, newchannels;
        split(input, oldchannels);

        for (int i = 0; i < 3; i++)
            newchannels[i] = oldchannels[i].clone().rowRange(thickness, input.rows - thickness).colRange(thickness, input.cols - thickness);
        
        merge(newchannels, result);
    }
    else if (input.channels() == 1)
        result = input.clone().rowRange(thickness, input.rows - thickness).colRange(thickness, input.cols - thickness);
        
    return result;
}

bool ncompare(string left, string right)
{
    bool pcont = false;
    char *cleft = (char *)left.c_str();
    char *cright = (char *)right.c_str();

    while (1)
    {
        pcont = false;

        if (strlen(cleft) == 0 || strlen(cright) == 0)
            return (strlen(cleft) != 0);

        if (isdigit(*cleft) && isdigit(*cright))
        {
            char *rleft;
            char *rright;
            long long valA = int(strtol(cleft, &rleft, 10));
            long long valB = int(strtol(cright, &rright, 10));

            if (valA != valB)
                return ((valA - valB) < 0);
            else if (int(rright - cright) != int(rleft - cleft))
                return (int(rright - cright) - int(rleft - cleft)) < 0;
            else
            {
                cleft = rleft;
                cright = rright;
                continue;
            }
        }
        if (isdigit(*cleft) || isdigit(*cright))
            return (isdigit(*cleft) != 0);
        
        while (*cleft && *cright)
        {
            if (isdigit(*cleft) || isdigit(*cright))
            {
                pcont = true;
                break;
            }
            if (tolower(*cleft) != tolower(*cright))
                return (tolower(*cleft) - tolower(*cright)) < 0;
            cleft++;
            cright++;
        }

        if (pcont)
            continue;

        return (*cleft == '\0');
    }
}

void cvutil::ForEachFileInPath(string path, void(*func)(string filename))
{
    vector<String> filenames;
    glob(path + "\\*.*", filenames);

    sort(filenames.begin(), filenames.end(), ncompare);
    /*vector<String> filenames2;
    glob(path + "\\*.png", filenames2);*/

    for (String& s : filenames)
        func(s);

    /*for (String& s : filenames2)
        func(s);*/
}

void cvutil::init(int argc, char *argv[], bool useOpt)
{
    // Enable the OpenCV to use hardware acceleration.
    setUseOptimized(useOpt);

    // Iniliatize Qt sub-system.
    GlobalValues::app = new QApplication(argc, argv);

    // Seed the random number generator
    srand(time(0));

}

void cvutil::drawText(Mat &GeomLayer, const string & text, Point org, Scalar color, int rightmargin, int thickness)
{
    Size sz;
    int baseline = 0;
    int fontname = FONT_HERSHEY_PLAIN;
    int fontsize = 5;

    Scalar s = color;

    int margin = GeomLayer.cols - rightmargin;
    sz = getTextSize(text, fontname, fontsize, thickness, &baseline);

    if ((sz.width + org.x) >= margin) // need to word wrap if the text exceeds the margin of the image.
    {
        // Split string based on spaces...
        vector<string> words;
        {
            string buffer;
            stringstream sstream(text);

            while (sstream >> buffer)
                words.push_back(buffer);
        }

        // Now group and make a list of concatenated the words with spaces enough so that
        // each concatenated string do not exceed the margin of the image.
        vector<string> clines;
        {
            ostringstream osstream("");

            for (auto str : words)
            {
                if (getTextSize(osstream.str() + str + " ", fontname, fontsize, thickness, &baseline).width >= margin)
                {
                    clines.push_back(osstream.str());
                    osstream.str("");
                    osstream << str << " ";
                }
                else
                    osstream << str << " ";
            }

            if (osstream.str().substr(0, osstream.str().length() - 1).length() != 0)
                clines.push_back(osstream.str().substr(0, osstream.str().length() - 1));
        }

        // Get the full text size of the 'word-wrapped' text
        // The text height is multipled by the number of lines, 
        // but we need the maximum text width across the lines.
        {
            Size s;
            sz.width = 0;
            sz.height *= clines.size();

            for (auto str : clines)
            {
                s = getTextSize(str, fontname, fontsize, thickness, &baseline);

                if (s.width > sz.width)
                    sz.width = s.width;
            }
        }

        // To perform actual drawing...
        for (int i = 0; i < clines.size(); i++)
        {
            Point p(org.x, org.y + sz.height * i);
            putText(GeomLayer, clines[i], p, fontname, fontsize, s, thickness, CV_AA);
        }

        // Finally update the size of the geometry drawn.
        // Width is already updated.
        sz.height *= clines.size();
    }
    else
        putText(GeomLayer, text, org, fontname, fontsize, s, thickness, CV_AA);
}

Mat cvutil::getImageFromComponents(Size sz, vector<vector<int>> components)
{
    Mat result = Mat::zeros(sz, CV_8UC1);
    unsigned char *data = result.ptr<unsigned char>();

    for (const auto &component : components)
        for (const auto &index : component)
            data[index] = 255;

    return result;
}

//void cvutil::setSwitch(bool on)
//{
//    GlobalValues::cswitch = on;
//}