/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: stdproto.h

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

#pragma once

#ifndef STDPROTO_H
#define STDPROTO_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <cmath>
#include <vector>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <tchar.h>
#include <experimental/filesystem>
#include <iostream>
#include <thread>
#include <sys/stat.h>

// Defaulting to Intel's AVX2 and FMA
#include <immintrin.h>

#include <Windows.h>

#include <cuda_runtime.h>

#ifdef WIN32
#if (defined _WINDLL && defined CVUTIL_SOURCE)
#define CVUTILAPI __declspec(dllexport) 
#include <QtWidgets/QApplication>
#include "cvutil_types.h"
#else
#define CVUTILAPI __declspec(dllimport)
#endif
#endif
//#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>
#include <crtdbg.h>

#define DEBUG_IN _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#define DEBUG_OUT _CrtDumpMemoryLeaks();
#define DEBUG_BREAK(x) \
_crtBreakAlloc = (x);

#else
#include <stdlib.h>
#endif

#endif
