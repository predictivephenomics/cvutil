/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: cvutil_linesim.cpp

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

// Ramer-Douglas-Peucker Line Simplifier
//
// Source:
// Ramer U (1972) An iterative procedure for the polygonal approximation of plane curves. 
// Computer graphics and image processing 1 (3):244-256
//
// Douglas DH, Peucker TK (1973) Algorithms for the reduction of the number of points 
// required to represent a digitized line or its caricature. Cartographica: The 
// International Journal for Geographic Information and Geovisualization 10 (2):112-122
//

#include "cvutil_linesim.h"

#include "profiler.h"

using namespace std;
using namespace cv;

vector<Point> doughlas_peucker(vector<Point> contour, double epsilon)
{
    double acoef, bcoef, ccoef, invdist, maxdist, maxadist = 0, adist, dist, distlongest = -1;
    int startidx = -1, stopidx = -1, maxidx = -1, updatept = 0, j;
    vector<int> stopstack;
    vector<Point> pixelstosave;
    Point start;
    
    if (contour.size() <= 1)
    {
        if (epsilon > 1)
            return pixelstosave;
        else
            return contour;
    }
    else if (contour.size() == 2)
    {
        if (norm(contour[0] - contour[1]) <= epsilon)
            return pixelstosave;
        else
            return contour;
    }

    //auto mloc = getmaxdist(contour);

    /*if (mloc.first <= epsilon)
    return pixelstosave;*/

    startidx = 0;
    contour.push_back(contour[0]);
    stopidx = static_cast<int>(contour.size()) - 1;
    updatept = 0;
    distlongest = 0;

    stopstack.reserve(500);
    pixelstosave.reserve(contour.size() + 1);
    pixelstosave.push_back(contour[startidx]);

    while (startidx < stopidx)
    {
        if (updatept == 0)
        {
            start = contour[startidx];
            stopstack.push_back(stopidx);
        }
        else
            start = contour[startidx];

        if (startidx == 0 && stopidx == contour.size() - 1)
        {
            for (j = (startidx + 1), maxadist = 0, maxidx = -1; j < stopidx; j++)
            {
                dist = ((contour[startidx].x - contour[j].x) * (contour[startidx].x - contour[j].x)) +
                    ((contour[startidx].y - contour[j].y) * (contour[startidx].y - contour[j].y));
                adist = fabs(dist);

                if (adist > maxadist)
                {
                    maxadist = adist;
                    maxdist = dist;
                    maxidx = j;
                }
            }

            //distlongest = (acoef * contour[maxidx].x - bcoef * contour[maxidx].y + ccoef) * invdist;
            //maxadist = fabs(dist);
            //distlongest *= -1;
            /*maxadist = mloc.first;
            maxidx = mloc.second;*/
        }
        else
        {
            acoef = contour[stopidx].y - start.y;
            bcoef = contour[stopidx].x - start.x;
            // We do not need ccoef as we are merely comparing the distances. 
            ccoef = contour[stopidx].x * start.y - contour[stopidx].y * start.x;

            // We probably do not need to use invdist to compute the exact distance
            // because we just want to know the index of the point with maximum distance.
            // Hence, we can just make comparisons without calculating invdist. Thus,
            // the following line is commented.
            invdist = 1.0 / sqrt(acoef * acoef + bcoef * bcoef);
            acoef *= invdist;
            bcoef *= invdist;
            ccoef *= invdist;
            
            for (j = (startidx + 1), maxadist = 0, maxidx = -1; j < stopidx; j++)
            {
                dist = (acoef * double(contour[j].x) - bcoef * double(contour[j].y) + ccoef);
                adist = fabs(dist);
                
                if (adist > maxadist)
                {
                    maxadist = adist;
                    maxidx = j;
                }
            }
        }

        if (maxidx != -1 && maxadist > epsilon)
        {
            stopidx = maxidx;
            updatept = 0;
        }
        else
        {
            // Add the stop pixels to save list.
            //if (startidx == 0 && stopidx == contour.size() - 1)
            pixelstosave.push_back(contour[stopidx]);
            //else
            //  pixelstosave.push_back(contour[stopidx]);

            // To update stopidx and update startidx.
            startidx = stopidx;
            if (stopstack.size() > 0)
                stopstack.pop_back();
            if (stopstack.size() > 0)
                stopidx = stopstack[stopstack.size() - 1];
            else
                stopidx = -1;
            updatept = 1;
        }
    }

    pixelstosave.pop_back();
    return pixelstosave;
}

Mat linesim_helper::linesim_st(Mat inputc, double epsilon)
{
    // We now find an 8-connected edge of all the components of the binary image.
    int ncontoursused = 0, hierarchy_level = 0, max_hierarchy_levels, isparent, nedge = 0, i, j;
    unsigned char *data = inputc.ptr<unsigned char>();
    vector<int> stopstack;
    vector<bool> parentdeleted, hierarchydelete;
    Point start;
    vector<Point> updatedcontour;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy, newhierarchy;

    // The border points are 4-connected because we used
    // 8-connectivity to identify the edge pixels (duality!).
    // We also have the information that the connected 
    // component under consideration does not have extremity 
    // conditions like touching the boundaries of the image.
    // This means that the edge points are 4-connected 
    // without exceptions. In other words, each point on the 
    // edge has atleast 2 neighboring pixels, not connected 
    // directly, but connected through the current pixel. If
    // the border pixel has only one neighboring pixel, then 
    // it must be an end point.

    // Each component has atleast one edge, that starts from 
    // one pixel and ends at the same pixel (because there 
    // are no boundary conditions). The component may have
    // multiple edges if it has multiple holes. So, we need
    // the list of edges for each component (first index),
    // then by the edge index (in case the component has 
    // holes).
    //tic();
    findContours(inputc.clone(), contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    // To find number of hierarchy_levels.
    for (i = 0, max_hierarchy_levels = 0; i < contours.size(); i++)
    {
        if (hierarchy[i][2] != -1) // Skip if the contour has a child contour.
            continue;

        hierarchy_level = 0;
        for (j = i; j != -1; j = hierarchy[j][3], hierarchy_level++);
        
        if (hierarchy_level > max_hierarchy_levels)
            max_hierarchy_levels = hierarchy_level;
    }

    parentdeleted.resize(max_hierarchy_levels);
    hierarchydelete.resize(hierarchy.size());
    for (i = 0; i < max_hierarchy_levels; i++)
        parentdeleted[i] = false;
    for (i = 0; i < hierarchy.size(); i++)
        hierarchydelete[i] = false;
    newhierarchy = hierarchy;

    // Get the contours as a list of hierarchy levels. Also, perform
    // line simplification operation as the list is built.
    // First index is hierarchy_level, second index is contour index.
    /*vector<vector<int>> hierarchies(max_hierarchy_levels);
    
    for (i = 0; i < max_hierarchy_levels; i++)
        hierarchies[i].reserve(contours.size() / 2);*/
    //qInfo() << "contours size = " << contours.size();
    for (i = 0, hierarchy_level = 0, isparent = 0; i < contours.size();)
    {
        if (isparent == 0)
        {
            // If parent contour is deleted.
            if (hierarchy_level > 0 && (parentdeleted[hierarchy_level - 1]))
            {
                // Mark the child contour to delete.
                parentdeleted[hierarchy_level] = true;
                hierarchydelete[i] = true;

                // Here we do not need to update the first child because
                // the parent is already marked to be deleted. Instead,
                // we just need to remove the first child reference.

                // Update previous contour id
                if (newhierarchy[i][1] != -1)
                {
                    newhierarchy[newhierarchy[i][1]][0] = newhierarchy[i][0];
                    //newhierarchy[i][1] = -1;
                }
                // Update next contour id
                if (newhierarchy[i][0] != -1)
                {
                    newhierarchy[newhierarchy[i][0]][1] = newhierarchy[i][1];
                    //newhierarchy[i][0] = -1;
                }
                newhierarchy[i][0] = -1;
                newhierarchy[i][1] = -1;

                // Update first child contour id
                /*if (newhierarchy[i][2] != -1)
                    newhierarchy[i][2] = -1;*/
            }
            else if (contours[i].size() > 2)
            {
                updatedcontour = doughlas_peucker(contours[i], epsilon);
                
                if (!updatedcontour.empty())
                    contours[i] = updatedcontour;
                
                // If the cuttrnt contour is empty, we need to delete it.
                // So first we mark it for deletion.
                parentdeleted[hierarchy_level] = updatedcontour.empty();
                hierarchydelete[i] = parentdeleted[hierarchy_level];

                // Then we update the adjacent contours in the same hierarchy
                // [next, previous] with -1.
                if (hierarchydelete[i])
                {
                    // Update the first child in parent contour
                    if (newhierarchy[i][3] != -1 && newhierarchy[newhierarchy[i][3]][2] == i)
                    {
                        newhierarchy[newhierarchy[i][3]][2] = newhierarchy[i][0];
                        newhierarchy[i][3] = -1;
                    }
                    // Update previous contour id
                    if (newhierarchy[i][1] != -1)
                    {
                        newhierarchy[newhierarchy[i][1]][0] = newhierarchy[i][0];
                        //newhierarchy[i][1] = -1;
                    }
                    // Update next contour id
                    if (newhierarchy[i][0] != -1)
                    {
                        newhierarchy[newhierarchy[i][0]][1] = newhierarchy[i][1];
                        //newhierarchy[i][0] = -1;
                    }

                    newhierarchy[i][0] = -1;
                    newhierarchy[i][1] = -1;
                    // Update first child contour id
                    //if (newhierarchy[i][2] != -1)
                    //    newhierarchy[i][2] = -1;
                }
            }
        }
        else
            parentdeleted[hierarchy_level + 1] = false;
        
        if (isparent == 0 && hierarchy[i][2] != -1)  // If the contour has an immediate child
        {
            i = hierarchy[i][2];    // Set the current contour to immediate child.
            hierarchy_level++;
            continue;
        }
        else
        {
            isparent = 0;

            // If all child contours are processed, go to next contour in the same hierarchy.
            if (hierarchy[i][0] != -1)  // If the contour has a next contour
            {
                i = hierarchy[i][0];    // Set the current contour to next contour.
                continue;
            }

            // If all next contours are processed, we then get back to parent contour.
            if (hierarchy[i][3] != -1)
            {
                i = hierarchy[i][3];    // Set the current contour to parent contour.
                hierarchy_level--;
                isparent = 1;
                continue;
            }

            break;
        }
    }
    
    // To draw the contours to result image after line simplification.
    Mat result = Mat::zeros(inputc.size(), CV_8UC1);;
    drawContours(result, contours, -1, Scalar(255), FILLED, 8, newhierarchy);
    
    contours.clear();
    hierarchy.clear();
    newhierarchy.clear();
    stopstack.clear();

    return result;
}
