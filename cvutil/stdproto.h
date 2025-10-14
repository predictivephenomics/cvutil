/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: stdproto.h

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

#pragma once

#ifndef STDPROTO_H
#define STDPROTO_H

// To disable warnings from external headers
#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/videoio.hpp>

#include <cmath>
#include <vector>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>
#endif

//#include <experimental/filesystem>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <sys/stat.h>

// Defaulting to Intel's AVX2 and FMA
#include <immintrin.h>

// #if defined(_MSC_VER) && !defined(__clang__)
// #define M2I32(x, idx) (x).m256i_i32[(idx)]
// #define M2F32(x, idx) (x).m256_f32[(idx)]
// #define M2U32(x, idx) (x).m256i_u32[(idx)]
// #elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
// #define M2I32(x, idx) (x)[(idx)]
// #define M2F32(x, idx) (x)[(idx)]
// #define M2U32(x, idx) (x)[(idx)]
// #endif

// #define _MM_FUNC_I32(func, v, m) \
//     (func)((v)[M2I32((m), 7)], (v)[M2I32((m), 6)], (v)[M2I32((m), 5)], (v)[M2I32((m), 4)], (v)[M2I32((m), 3)], (v)[M2I32((m), 2)], (v)[M2I32((m), 1)], (v)[M2I32((m), 0)])
// #define _MM_FUNC_I32_STEP(func, v, m, s) \
//     (func)((v)[M2I32((m), 7) + (s)], (v)[M2I32((m), 6) + (s)], (v)[M2I32((m), 5) + (s)], (v)[M2I32((m), 4) + (s)], (v)[M2I32((m), 3) + (s)], (v)[M2I32((m), 2) + (s)], (v)[M2I32((m), 1) + (s)], (v)[M2I32((m), 0) + (s)])
// #define _MM_FUNC_F32(func, v, m) \
//     (func)((v)[M2F32((m), 7)], (v)[M2F32((m), 6)], (v)[M2F32((m), 5)], (v)[M2F32((m), 4)], (v)[M2F32((m), 3)], (v)[M2F32((m), 2)], (v)[M2F32((m), 1)], (v)[M2F32((m), 0)])

//#include <Windows.h>

// UI facilities
#include <QtWidgets/QApplication>
#include <QtWidgets/QtWidgets>

#ifdef _MSC_VER
#if (defined _WINDLL && defined CVUTIL_SOURCE)
#define CVUTILAPI __declspec(dllexport) 
#else
#define CVUTILAPI __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define CVUTILAPI __attribute__((visibility("default")))
#endif
//#endif

#if defined(_DEBUG) && (defined(_WIN32) || defined(_WIN64))
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

#pragma warning(pop)

#define CVUTIL_SQRT2 1.414213562373095048801688724209

#endif
