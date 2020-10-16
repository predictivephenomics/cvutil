/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: main.h

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

#pragma once

#ifndef MAIN_H
#define MAIN_H

#include "cvutil.h"

using namespace cvutil;

template<typename T>
using ListRef = std::vector<T>&;

typedef ListRef<int> IndexList;
typedef std::vector<int> Indices;
typedef ListRef<double> ValueList;
typedef std::vector<double> Values;

typedef ListRef<cv::Point> PointList;
typedef std::vector<cv::Point> Points;

template<typename T>
using ListofListsRef = std::vector<std::vector<T>>&;

template<typename T>
using ListofLists = std::vector<std::vector<T>>;

template<typename T>
using ListofListsPtr = std::vector<std::vector<T>>*;

template<typename TKey, typename TVal>
using Dictionary = std::unordered_map<TKey, TVal>;

template<typename TKey, typename TVal>
using DictionaryRef = std::unordered_map<TKey, TVal>&;

#endif
