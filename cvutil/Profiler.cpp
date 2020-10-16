/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: Profiler.cpp

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

#include "cvutil_types.h"
#include "profiler.h"

using namespace std;
using namespace cvutil;

clock_t GlobalValues::start = 0;
clock_t GlobalValues::stop = 0;

void cvutil::tic()
{
    GlobalValues::start = clock();
}

void cvutil::toc()
{
    GlobalValues::stop = clock();
    cout << "Processing time : " << double(GlobalValues::stop - GlobalValues::start + 1) / double(CLOCKS_PER_SEC) << " seconds.\n";
}

void cvutil::toc(std::string message, int line)
{
    GlobalValues::stop = clock();

    if (line == -1)
        cout << message << " : " << double(GlobalValues::stop - GlobalValues::start + 1) / double(CLOCKS_PER_SEC) << " seconds.\n";
    else
        cout << "[" << line << "]:" << message << " : " << double(GlobalValues::stop - GlobalValues::start + 1) / double(CLOCKS_PER_SEC) << " seconds.\n";
}

void cvutil::toc(double& timeelapsed)
{
    GlobalValues::stop = clock();
    timeelapsed = double(GlobalValues::stop - GlobalValues::start + 1) / double(CLOCKS_PER_SEC);
}
