/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: main.cpp

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

// Code for testing the software. The code written here is not used elsewhere nor 
// exported to shared library.

#include "profiler.h"
#include "cvutil.h"

// Program headers
#include "main.h"

//#ifdef _DEBUG
#define DEBUG_IMAGES
//#endif

#define DEBUG_MESSAGES

using namespace std;
using namespace cv;
using namespace cvutil;

#include <QtWidgets/QApplication>
#include "cvutil_types.h"

QApplication *GlobalValues::app = nullptr;

void extractfeatures(string filename)
{
    string ccfilename = filename.substr(0, filename.length() - 4) + "_cc.png";
    string cofilename = filename.substr(0, filename.length() - 4) + "_co.png";
    string skfilename = filename.substr(0, filename.length() - 4) + "_sk.png";
    string dtfilename = filename.substr(0, filename.length() - 4) + "_dt.png";
    string lsfilename = filename.substr(0, filename.length() - 4) + "_ls.png";
    string tofilename = filename.substr(0, filename.length() - 4) + "_to.png";
    string thfilename = filename.substr(0, filename.length() - 4) + "_th.png";

    // Overlapped images
    string lsdtfilename = filename.substr(0, filename.length() - 4) + "_lsdt.png";
    string lsskfilename = filename.substr(0, filename.length() - 4) + "_lssk.png";
    string skovfilename = filename.substr(0, filename.length() - 4) + "_skov.png";
    string ccovfilename = filename.substr(0, filename.length() - 4) + "_ccov.png";
    string lstofilename = filename.substr(0, filename.length() - 4) + "_lsto.png";
    string lsthfilename = filename.substr(0, filename.length() - 4) + "_lsth.png";

    cout << "Processing file : " << experimental::filesystem::path(filename).filename() << endl;

    Mat img = im2float(imread(filename, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR));
    Mat out, ske;

    tic();
    // To segment the image by thresholding...
    out = getSingleChannel(img < 0.79f, CV_8UC1);
    
    vector<vector<int>> comps = getConnectedComponents(out, 500);
    Mat matlist = getImageFromComponents(out.size(), comps);
    //Mat matlist = getLargestConnectedComponent(out);
    Mat lsim = linesim(matlist, LineSimplificationType::DouglasPeucker, 2.0);
    comps = getConnectedComponents(lsim, 500);
    lsim = getImageFromComponents(lsim.size(), comps);
    toc("Time taken to perform line smoothing");
    
    tic();
    //Mat skeleton = bwthin(matlist);
    //Mat lsimskeleton = bwthin(lsim);
    Mat skel = bwskel(matlist);
    toc("Time taken to thin the segmented images");

#ifdef DEBUG_IMAGES
    imwrite(ccfilename, matlist);
    imwrite(lsfilename, lsim);
    imwrite(thfilename, skel);
    /*imwrite(skfilename, skeleton);
    imwrite(lsskfilename, lsimskeleton);*/
#endif

    /*Points branchpts, endpts, overlappts;
    ListofLists<Point> rootsegments;
    Points branchptslsim, endptslsim, overlapptslsim;
    ListofLists<Point> rootsegmentslsim;

    tic();
    getroottopology(skeleton, branchpts, endpts, overlappts, rootsegments);
    getroottopology(lsimskeleton, branchptslsim, endptslsim, overlapptslsim, rootsegmentslsim);
    toc("Time taken to generate root topology");

    tic();
    double rootlength = getrootlength(skeleton, rootsegments, overlappts);
    double lsrootlength = getrootlength(lsimskeleton, rootsegmentslsim, overlapptslsim);
    toc("Time taken to compute total root length");

    tic();
    int npixels = countNonZero(skeleton), lsnpixels = countNonZero(lsimskeleton);

    cout << "Original:\n";
    cout << "Root length: " << rootlength << endl;
    cout << "Root length (in cm): " << (rootlength * 2.54) / 600.0 << endl;
    cout << "Total number of segments: " << rootsegments.size() << endl;
    cout << "Total branch points: " << branchpts.size() << endl;
    cout << "Total end points: " << endpts.size() << endl;
    cout << "Total overlapped points: " << overlappts.size() << endl;
    cout << "Total number of points in skeleton matrix: " << npixels << endl;
    cout << "Root length (in cm) using number of points: " << (npixels * 2.54) / 600.0 << endl;
    Mat topimage = drawtopology2(matlist, skeleton, branchpts, endpts, overlappts);*/

    /*if (validatesegments(skeleton, branchpts, endpts, overlappts, rootsegments))
        cout << "All OK!\n";
    else
        cout << "Not OK!\n";*/
    
    /*cout << "Line smoothed:\n";
    cout << "Root length: " << lsrootlength << endl;
    cout << "Root length (in cm): " << (lsrootlength * 2.54) / 600.0 << endl;
    cout << "Total number of segments: " << rootsegmentslsim.size() << endl;
    cout << "Total branch points: " << branchptslsim.size() << endl;
    cout << "Total end points: " << endptslsim.size() << endl;
    cout << "Total overlapped points: " << overlapptslsim.size() << endl;
    cout << "Total number of points in skeleton matrix: " << lsnpixels << endl;
    cout << "Root length (in cm) using number of points: " << (lsnpixels * 2.54) / 600.0 << endl;
    Mat lsimtopimage = drawtopology2(lsim, lsimskeleton, branchptslsim, endptslsim, overlapptslsim);*/
    /*if (validatesegments(lsimskeleton, branchptslsim, endptslsim, overlapptslsim, rootsegmentslsim))
        cout << "All OK!\n";
    else
        cout << "Not OK!\n";*/
    
/*
    toc("Time taken to validate root topology");

    tic();
    ListofLists<double> radii, lsimradii;
    Values avgradii, lsimavgradii, bins, lsimbins;
    Indices y, lsimy;

    getrootradii(skeleton, matlist, rootsegments, radii);
    getrootradii(lsimskeleton, lsim, rootsegmentslsim, lsimradii);

    getrootradiiforsegments(radii, avgradii);
    getrootradiiforsegments(lsimradii, lsimavgradii);

    gethistogramofrootradii(skeleton, matlist, bins, y);
    gethistogramofrootradii(lsimskeleton, lsim, lsimbins, lsimy);
    
    double minval = min(*min_element(begin(bins), end(bins)), *min_element(begin(lsimbins), end(lsimbins)));
    double maxval = max(*max_element(begin(bins), end(bins)), *max_element(begin(lsimbins), end(lsimbins)));
    toc("Time taken to determine radius profiles");

    int nbins = 20;
    double interval = (maxval - minval) / double(nbins);
    vector<Indices> yvals(2);
    Values xvals;
    int testcount;
    double bin, prevbin = 0.0;

    xvals.resize(nbins);
    yvals[0].resize(nbins);
    yvals[1].resize(nbins);

    for (int i = 0; i < nbins; i ++)
    {
        xvals[i] = minval + (double(i) + 0.5) * interval;
        bin = minval + double(i + 1) * interval;
        testcount = 0;

        for (int j = 0; j < bins.size(); j++)
            if (bins[j] > prevbin && bins[j] <= bin)
                testcount += y[j];

        yvals[0][i] = testcount;

        testcount = 0;

        for (int j = 0; j < lsimbins.size(); j++)
            if (lsimbins[j] > prevbin && lsimbins[j] <= bin)
                testcount += lsimy[j];

        yvals[1][i] = testcount;
        prevbin = bin;
    }
    
    figure f1;
    f1.subplot(2, 1, 1);
    f1.bar(xvals, yvals, {"Without line smoothing", "With line smoothing"});
    f1.title("Histogram of root radii");
    f1.xlabel("Root radii");
    f1.ylabel("Histogram counts");

    f1.subplot(2, 1, 2);
    f1.imshow(matlist.rowRange(1300, 2301).colRange(1300, 3501));
    f1.title("Histogram of root radii 2");
    f1.wait();
*/
    

//#ifdef DEBUG_IMAGES
//    imwrite(tofilename, topimage);
//    imwrite(lstofilename, lsimtopimage);
//#endif
    //window("topimage", topimage);

    Mat labels, labelsls;
    Mat dist = im2double(bwdist(matlist, labels)) * 255;
    Mat lsdist = im2double(bwdist(lsim, labelsls)) * 255;
    toc();

//#ifdef DEBUG_IMAGES
//    vector<Mat> channels;
//    Mat result;
//
//    channels.push_back(Mat::zeros(matlist.size(), CV_64FC1));
//    channels.push_back(dist);
//    channels.push_back(im2double(skeleton) * 255);
//    merge(channels, result);
//    imwrite(dtfilename, result);
//
//    channels.clear();
//    channels.push_back(Mat::zeros(matlist.size(), CV_64FC1));
//    channels.push_back(lsdist);
//    channels.push_back(im2double(lsimskeleton) * 255);
//    merge(channels, result);
//    imwrite(lsdtfilename, result);
//
//    channels.clear();
//    channels.push_back(Mat::zeros(matlist.size(), CV_64FC1));
//    channels.push_back(im2double(out) * 255);
//    channels.push_back(im2double(lsim) * 255);
//    merge(channels, result);
//    imwrite(ccovfilename, result);
//
//    channels.clear();
//    channels.push_back(Mat::zeros(matlist.size(), CV_64FC1));
//    channels.push_back(im2double(skeleton) * 255);
//    channels.push_back(im2double(lsimskeleton) * 255);
//    merge(channels, result);
//    imwrite(skovfilename, result);
//#endif
}

#include "demo.h"

void makeplot()
{
    vector<vector<float>> areas;
    vector<string> groups;

    for (auto & p : experimental::filesystem::directory_iterator(experimental::filesystem::current_path()))
    {
        vector<float> listvals;

        if (p.path().extension() == ".txt")
        {
            float f = 0;

            groups.push_back(p.path().stem().string());

            ifstream m;
            m.open(p.path().filename());
            while (m >> f)
            {
                listvals.push_back(f);
            }
            m.close();
            
            areas.push_back(listvals);
        }
    }

    if (groups.size() == 0)
        return;

    double minval = min(*min_element(begin(areas[0]), end(areas[0])), *min_element(begin(areas[1]), end(areas[1])));
    double maxval = max(*max_element(begin(areas[0]), end(areas[0])), *max_element(begin(areas[1]), end(areas[1])));

    sort(areas[0].begin(), areas[0].end());
    sort(areas[1].begin(), areas[1].end());

    unordered_map<float, int> a1, a2;

    for (int i = 0; i < areas[0].size(); i++)
        a1[areas[0][i]]++;
    for (int i = 0; i < areas[1].size(); i++)
        a2[areas[1][i]]++;

    vector<float> a1b, a2b;
    vector<int> a1y, a2y;

    for (auto i : a1)
    {
        a1b.push_back(i.first);
        a1y.push_back(i.second);
    }

    for (auto i : a2)
    {
        a2b.push_back(i.first);
        a2y.push_back(i.second);
    }

    int nbins = 5;
    double interval = (maxval - minval) / double(nbins);
    vector<Indices> yvals(2);
    Values xvals;
    int testcount;
    double bin, prevbin = 0.0;

    xvals.resize(nbins);
    yvals[0].resize(nbins);
    yvals[1].resize(nbins);

    for (int i = 0; i < nbins; i++)
    {
        xvals[i] = minval + (double(i) + 0.5) * interval;
        bin = minval + double(i + 1) * interval;
        testcount = 0;

        for (int j = 0; j < a1b.size(); j++)
            if (a1b[j] > prevbin && a1b[j] <= bin)
                testcount += a1y[j];

        yvals[0][i] = testcount;

        testcount = 0;

        for (int j = 0; j < a2b.size(); j++)
            if (a2b[j] > prevbin && a2b[j] <= bin)
                testcount += a2y[j];

        yvals[1][i] = testcount;
        prevbin = bin;
    }

    Ptr<figure> f;

    while (true)
    {
        try
        {
            f = createfigure();
        }
        catch (exception e) {}

        if (f != nullptr)
            break;
    }

    f->bar(xvals, yvals, groups);
    f->title("Histogram of hand areas");
    f->xlabel("Hand Area (sq. cm)");
    f->ylabel("Frequency");
    f->wait();

    f->release();
}

void processimage(string filename)
{
    //demo(filename);
}

int main(int argc, char *argv[])
{
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\new_setup_images\\round2\\I_9780444505156_1.png";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\b2.jpg";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\b.png";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\p1-CR2-1-1.jpg";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\p1-CR1-3.jpg";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\p1-SR-3.jpg";
    //string filename = "C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\working2\\p5-PR-1.jpg";

    init(argc, argv);
    
    /*cout << getBuildInformation() << endl;
    system("pause");*/
    tic();

    //demo("C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\demo\\I_1014,5_3.png");
    //demo("C:\\Users\\aseethepalli\\Documents\\projects\\AltRhizo Lite\\demo\\I_1038,3_3.png");
    
    //makeplot();

    //liveview();
    ForEachFileInPath("R:/Root Phenomics Lab/Images/gantrytestthird/gantryphotoscantestjpeg/", processimage);
    //toc("Total time taken");

    //system("pause");
    return 0;
}

