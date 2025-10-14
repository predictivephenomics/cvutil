/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: Profiler.cpp

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
