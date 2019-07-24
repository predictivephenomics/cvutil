/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: video.h

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

#ifndef VIDEO_H
#define VIDEO_H

#include "cvutil.h"

using namespace cvutil;

namespace cvutil
{
    class CVUTILAPI IVWriter
    {
    public:
        virtual void open(cv::Size framesize, double fps) = 0;
        virtual void open(std::string filepath, cv::Size framesize, double fps) = 0;
        virtual bool IsOpened() = 0;
        virtual void write(cv::Mat frame) = 0;
        virtual void close() = 0;
    };

    enum class VideoWriterType {Default, Image, X264};

    CVUTILAPI cv::Ptr<IVWriter> createVideoWriter(VideoWriterType type = VideoWriterType::Default);
    CVUTILAPI cv::Ptr<IVWriter> createVideoWriter(std::string filepath, VideoWriterType type = VideoWriterType::Default);
}

#endif
