/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: cvutil_types.h

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
