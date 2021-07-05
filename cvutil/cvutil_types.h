/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_types.h

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

#pragma once

#ifndef CVUTIL_TYPES_H
#define CVUTIL_TYPES_H

#include "stdproto.h"

namespace cvutil
{
    struct WinData
    {
        cv::Mat m;
        std::string winname;
        HWND hWnd;

        // To be used only by the event handler
        bool panmode = false;
        int lbuttondownx = -1, lbuttondowny = -1;
        int zoom = 100;

        // To be used in both event handler and the window function
        int xzoomshift = 0, yzoomshift = 0;
        int xsize, ysize;
    };

    class GlobalValues
    {
    public:
        // For use with profiling operations
        static clock_t start;
        static clock_t stop;

        // Qt specific
        static QApplication *app;

        //static bool cswitch;

        // High-GUI specific
        static std::vector<WinData *> figures;
    };
}

#endif
