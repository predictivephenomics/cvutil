/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: cvutil_videowriter.cpp

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

#include "cvutil_videowriter.h"

#include <cstring>

using namespace std;
using namespace cv;

DefaultVideoWriter::DefaultVideoWriter() {}

DefaultVideoWriter::DefaultVideoWriter(string filepath)
{
    this->filepath = filepath;
}

void DefaultVideoWriter::setfilepath(string filepath)
{
    this->filepath = filepath;
}

void DefaultVideoWriter::open(Size framesize, double fps)
{
    writer.open(filepath, fourcc, fps, framesize, true);
    opened = true;
}

void DefaultVideoWriter::open(string filepath, Size framesize, double fps)
{
    this->filepath = filepath;
    writer.open(filepath, fourcc, fps, framesize, true);
    opened = true;
}

bool DefaultVideoWriter::IsOpened()
{
    return opened;
}

void DefaultVideoWriter::write(Mat frame)
{
    Mat videoframe;

    // The video writer from OpenCV does not support 
    // writing single channel images to video frames.
    // So, we replicate the image into 3-channel image
    // and then write the video frame.
    if (frame.channels() == 1)
    {
        vector<Mat> ch;
        for (int i = 0; i < 3; i++)
            ch.push_back(frame);
        merge(ch, videoframe);
    }
    else
        videoframe = frame.clone();

    if (videoframe.depth() != 8)
        videoframe.convertTo(videoframe, CV_8UC3);
    
    writer.write(videoframe);
}

void DefaultVideoWriter::close()
{
    writer.release();
}

FrameWriter::FrameWriter() {}

FrameWriter::FrameWriter(string filepath)
{
    this->filepath = filepath;
}

void FrameWriter::setfilepath(string filepath)
{
    this->filepath = filepath;
}

void FrameWriter::open(Size framesize, double fps)
{
    basefilename = filepath.substr(0, filepath.length() - 4);
    fileext = filepath.substr(filepath.length() - 3);
    opened = true;
}

void FrameWriter::open(string filepath, Size framesize, double fps)
{
    this->filepath = filepath;
    basefilename = filepath.substr(0, filepath.length() - 4);
    fileext = filepath.substr(filepath.length() - 3);
    opened = true;
}

bool FrameWriter::IsOpened()
{
    return opened;
}

void FrameWriter::write(Mat frame)
{
    Mat videoframe;

    // The video writer from OpenCV does not support 
    // writing single channel images to video frames.
    // So, we replicate the image into 3-channel image
    // and then write the video frame.
    if (frame.channels() == 1)
    {
        vector<Mat> ch;
        for (int i = 0; i < 3; i++)
            ch.push_back(frame);
        merge(ch, videoframe);
    }
    else
        videoframe = frame.clone();

    if (videoframe.depth() != 8)
        videoframe.convertTo(videoframe, CV_8UC3);

    imwrite(basefilename + to_string(framenumber) + "." + fileext, videoframe);
    framenumber++;
}

void FrameWriter::close()
{
    filepath = "";
    basefilename = "";
    fileext = "";
    opened = false;
    framenumber = 1;
}

CVUTILAPI Ptr<IVWriter> cvutil::createVideoWriter(VideoWriterType type)
{
    switch (type)
    {
    case VideoWriterType::Default:
        return makePtr<DefaultVideoWriter>();
    case VideoWriterType::Image:
        return makePtr<FrameWriter>();
    default:
        throw exception("Unknown video format specified.");
    }
}

CVUTILAPI Ptr<IVWriter> cvutil::createVideoWriter(string filepath, VideoWriterType type)
{
    switch (type)
    {
    case VideoWriterType::Default:
        return makePtr<DefaultVideoWriter>(filepath);
    case VideoWriterType::Image:
        return makePtr<FrameWriter>(filepath);
    default:
        throw exception("Unknown video format specified.");
    }
}
