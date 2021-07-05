/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: cvutil_videowriter.cpp

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
