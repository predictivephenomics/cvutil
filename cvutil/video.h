/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: video.h

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
