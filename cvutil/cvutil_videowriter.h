/*  Copyright (C) 2018-2019 Noble Research Institute, LLC

File: cvutil_videowriter.h

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

#ifndef CVUTIL_VIDEOWRITER_H
#define CVUTIL_VIDEOWRITER_H

#include "cvutil.h"

namespace cvutil
{

    class DefaultVideoWriter : public IVWriter
    {
        std::string filepath;
        cv::VideoWriter writer;
        bool opened = false;

        // By default, the video writer from OpenCV 
        // generates an AVI file using an available encoder
        // from Windows/Linux. This may result in not able 
        // to generate videos in MP4 format, which takes 
        // a lot lesser space. The FFMPEG library needs
        // a FourCC code to identify the right encoder
        // for a given file format. But this FourCC code is
        // not used when the file format is MP4 as there are
        // several MP4 sub types for various applications. To 
        // successfully make an MP4 file, we need to provide
        // the object type in place of FourCC code, when 
        // specifying the file extension of the output file
        // as ".mp4". The object type for default MP4 is 0x20,
        // which can be found in the source code of 
        // libavformat/isom.c. The variable "ff_mp4_obj_type" 
        // contains a list of object types supported by 
        // FFmpeg. For more information, visit the following 
        // site: http://mp4ra.org/#/object_types
        int fourcc = 0x20;

    public:
        DefaultVideoWriter();
        DefaultVideoWriter(std::string filepath);
        void setfilepath(std::string filepath);
        void open(cv::Size framesize, double fps);
        void open(std::string filepath, cv::Size framesize, double fps);
        bool IsOpened();
        void write(cv::Mat frame);
        void close();
    };

    // Writes individual frames to disk instead of
    // video. The file name specified using the 
    // constructor or the function setfilepath()
    // is appended with 1,2,3... etc to mark the
    // sequence of frames saved on disk. Any 
    // existing file in the destination location 
    // will get overwritten. Frame size and FPS 
    // are ignored. Only PNG and JPG files can 
    // be saved.
    class FrameWriter : public IVWriter
    {
        std::string filepath;
        std::string basefilename;
        std::string fileext;
        bool opened = false;
        int framenumber = 1;

    public:
        FrameWriter();
        FrameWriter(std::string filepath);
        void setfilepath(std::string filepath);
        void open(cv::Size framesize, double fps);
        void open(std::string filepath, cv::Size framesize, double fps);
        bool IsOpened();
        void write(cv::Mat frame);
        void close();
    };
}
#endif
