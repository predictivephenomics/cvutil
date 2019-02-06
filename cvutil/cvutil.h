/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: cvutil.h

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

#ifndef CVUTIL_H
#define CVUTIL_H

#include "stdproto.h"

#define CVUTIL_VERSION_MAJOR    1
#define CVUTIL_VERSION_MINOR    3

#define CV_ERROR2( Code, Msg )                                       \
{                                                                    \
    cvError( (Code), __func__, Msg, __FILE__, __LINE__ );            \
    exit(1);                                                         \
}

#define CV_ASSERT2( Condition, Msg )                                     \
{                                                                        \
    if( !(Condition) )                                                   \
        CV_ERROR2( CV_StsInternal, Msg );                                \
}

#include "profiler.h"
#include "cvutil_core.h"
#include "cvutil_matlab_interface.h"
#include "video.h"
#include "cvutil_templates.h"


#endif
