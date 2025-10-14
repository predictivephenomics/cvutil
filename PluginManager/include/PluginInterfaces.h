/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: PluginInterfaces.h

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

#ifndef PLUGININTERFACES_H
#define PLUGININTERFACES_H

// To disable warnings from external headers.
#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#include <QtWidgets/QtWidgets>
#include <QtCharts/QtCharts>
#pragma warning(pop)

#ifdef _WIN64
#if (!defined PLUGINAPI)
#if (defined PLUGINMANAGER_SOURCE)
#define PLUGINAPI __declspec(dllexport) 
#else
#define PLUGINAPI __declspec(dllimport)
#endif
#endif
#endif

enum class ParameterType { None, Boolean, Integer, Float, IntegerRange, FloatRange, IntegerSpan, Items };
enum class AppType { None, Custom, Segmentation };
enum class OutputType { Image, Images, Values, ImageAndValues, ImagesAndValues };

class IParameterInfo
{
public:
    virtual std::string getName() = 0;
    virtual std::string getDisplayText() = 0;
    virtual ParameterType getParameterType() = 0;
};

class Parameter : public IParameterInfo
{
protected:
    std::string Name;
    std::string DisplayText;
    ParameterType type = ParameterType::None;
    std::string stylesheet = "";

    Parameter(std::string _Name, std::string _DisplayText)
    {
        Name = _Name;
        DisplayText = _DisplayText;
    }

public:
    std::function<bool()> enabled = []() { return true; };

public:
    virtual std::string getName() { return Name; };
    virtual std::string getDisplayText() { return DisplayText; };
    virtual ParameterType getParameterType() { return type; };
    virtual void setEnabledFunction(std::function<bool()> func) { enabled = func; };
    virtual void setStyleSheet(std::string str) { stylesheet = str; }
};

class BooleanParameter : public Parameter
{
protected:
    bool value;
public:
    BooleanParameter(std::string _Name, std::string _DisplayText) :
        Parameter(_Name, _DisplayText)
    {
        type = ParameterType::Boolean;
        value = false;
    }

    virtual bool getValue() { return value; };
    virtual void setValue(bool _value) { value = _value; };
};

Q_DECLARE_METATYPE(BooleanParameter*);

template <typename T>
class ValueParameter : public Parameter
{
protected:
    T value, minVal, maxVal;
public:
    ValueParameter(std::string _Name, std::string _DisplayText, T _minVal, T _maxVal) :
        Parameter(_Name, _DisplayText)
    {
        if (typeid(T) == typeid(int))
            type = ParameterType::Integer;
        else if (typeid(T) == typeid(float))
            type = ParameterType::Float;
        else
            throw (std::string("In function:") + std::string(__func__) + std::string(" - Unknown value parameter."));

        if (_maxVal <= _minVal)
            throw (std::string("In function:") + std::string(__func__) + std::string(" - maxVal must be greater than minVal."));

        minVal = _minVal;
        maxVal = _maxVal;
        value = minVal;
    }

    virtual T getValue() { return value; };
    virtual void setValue(T _value) { value = _value; };
    virtual T getMinValue() { return minVal; };
    virtual T getMaxValue() { return maxVal; };
};

typedef ValueParameter<int> IntegerParameter;
typedef ValueParameter<float> FloatParameter;

Q_DECLARE_METATYPE(IntegerParameter*);
Q_DECLARE_METATYPE(FloatParameter*);

template <typename T>
class ValueRangeParameter : public ValueParameter<T>
{
public:
    ValueRangeParameter(std::string _Name, std::string _DisplayText, T _minVal, T _maxVal) :
        ValueParameter<T>(_Name, _DisplayText, _minVal, _maxVal)
    {
        if (typeid(T) == typeid(int))
            ValueParameter<T>::type = ParameterType::IntegerRange;
        else if (typeid(T) == typeid(float))
            ValueParameter<T>::type = ParameterType::FloatRange;
        else
            throw (std::string("In function:") + std::string(__func__) + std::string("Unknown value parameter."));

        if (_maxVal <= _minVal)
            throw (std::string("In function:") + std::string(__func__) + std::string("maxVal must be greater than minVal."));

        ValueParameter<T>::minVal = _minVal;
        ValueParameter<T>::maxVal = _maxVal;
        ValueParameter<T>::value = _minVal;
    }
};

typedef ValueRangeParameter<int> IntegerRangeParameter;
typedef ValueRangeParameter<float> FloatRangeParameter;

Q_DECLARE_METATYPE(IntegerRangeParameter*);
Q_DECLARE_METATYPE(FloatRangeParameter*);

template <typename T>
class LimitRangeParameter : public ValueParameter<T>
{
protected:
    T alt_value;
public:
    LimitRangeParameter(std::string _Name, std::string _DisplayText, T _minVal, T _maxVal) :
        ValueParameter<T>(_Name, _DisplayText, _minVal, _maxVal)
    {
        if (typeid(T) == typeid(int))
            ValueParameter<T>::type = ParameterType::IntegerSpan;
        /*else if (typeid(T) == typeid(float))
            ValueParameter<T>::type = ParameterType::FloatRange;*/
        else
            throw (std::string("In function:") + std::string(__func__) + std::string("Unknown value parameter."));

        if (_maxVal <= _minVal)
            throw (std::string("In function:") + std::string(__func__) + std::string("maxVal must be greater than minVal."));

        ValueParameter<T>::minVal = _minVal;
        ValueParameter<T>::maxVal = _maxVal;
        ValueParameter<T>::value = _minVal;
        alt_value = _maxVal;
    }

    virtual void setValue(T _value)
    {
        if (_value > alt_value)
            throw (std::string("In function:") + std::string(__func__) + std::string("Lower limit is less than upper limit."));
        ValueParameter<T>::value = _value;
    }
    virtual T alt_getValue() { return alt_value; }
    virtual void alt_setValue(T _value)
    {
        if (_value < ValueParameter<T>::value)
            throw (std::string("In function:") + std::string(__func__) + std::string("Upper limit is less than lower limit."));
        alt_value = _value;
    }
};

typedef LimitRangeParameter<int> IntegerSpanParameter;

Q_DECLARE_METATYPE(IntegerSpanParameter*);

class ItemsParameter : public Parameter
{
    std::vector<std::string> items;
    int selectedindex;

public:
    ItemsParameter(std::string _Name, std::string _DisplayText, std::vector<std::string> _items) :
        Parameter(_Name, _DisplayText)
    {
        items = _items;
        selectedindex = 0;
        type = ParameterType::Items;
    }

    std::vector<std::string> getItems() { return items; };
    int getValue() { return selectedindex; };
    void setValue(int index) { selectedindex = index; }
};

Q_DECLARE_METATYPE(ItemsParameter*);

class IPlugin// : public QObject
{
    //Q_OBJECT;
public:
    virtual ~IPlugin() {}
    virtual std::string getName() = 0;
    virtual AppType getApplicationType() = 0;
    virtual OutputType getOutputType() = 0;
    virtual std::string getAlgorithmName() = 0;
    virtual std::vector<IParameterInfo *> getParameters() = 0;

    // A plugin can have two ways to communicate with the main program:
    // 1. Using parameters. When the parameters of the plugin are defined,
    //    the program automatically generates appropriate UI for each 
    //    parameter.
    // 2. If the plugin has complex parameters, the plugin can define its 
    //    own UI and pass the UI to the main program. A plugin can have 
    //    multiple instances of UI, but it belongs to the same plugin 
    //    object.
    virtual QWidget *getPluginUI() = 0;
    virtual void setImage(cv::Mat image, QString imagename = "") = 0;
    virtual void execute() = 0;
    //virtual void clearOutput() = 0;

    // A plugin can tell which image to show in the main window. If
    // The function below returns zero, then the main window shows 
    // the input image. Otherwise it shows the index of one of the 
    // output images generated by the plugin.
    virtual int getDisplayIndex() = 0;
    virtual void setDisplayIndex(int idx) = 0;

    // Returns 
    virtual std::vector<cv::Mat> getOutputImages() = 0;
    virtual std::vector<double> getFeatures() = 0;

    // Return features for a specific ROI index.
    virtual std::vector<double> getFeatures(int roiidx) = 0;
    
    // Inform the plugin that the user selected batch mode
    virtual void setBatchMode(bool on) = 0;

    // To save the output from each image is done by the following function.
    // The return value is the save status. If any error is observed while 
    // saving then the plugin may log a message and return 1, otherwise
    // The function should return 0 for successful save.
    virtual int saveOutput(QString savloc, QString filename) = 0;

    virtual void saveMetadata(QString imgloc, QString savloc, QString filepath = "") = 0;
    virtual void loadMetadata(QString filepath) = 0;
    // The function is used to write column names before writing the features.
    virtual int writeHeader(QString savloc, QString filename = "") = 0;

    // Needed by the main program to query the column names of the values 
    // extracted for display.
    virtual std::vector<QString> getCSVColumnNames() = 0;

    // Get chart widget for visualization. This is invoked when the 
    // plugin is operating in interactive mode. The plugin may return a
    // nullptr if it has no visualization.
    virtual QChart *getChart() = 0;

    // This function is used to inform the host window how many steps the 
    // execute function takes for analyzing an input image, in order to 
    // display the progress in progress dialog. If the number of steps 
    // are set to less than or equal to zero, the progress dialog is not 
    // shown.
    virtual int getProgressSteps() = 0;

    // Enables the user to abort an analysis that is taking longer time.
    // It works only if the plugin supports multiple progress steps and 
    // updates progress to the main window using updateProgress().
    virtual void abort() = 0;
signals:
    // To be used as signal to the main application to update the image shown.
    virtual void updateVisualOutput(cv::Mat m) = 0;

    // To be used as signal to the main application to update the progress 
    // of analyzing an image in interactive mode.
    virtual void updateProgress(QString status) = 0;
};

QT_BEGIN_NAMESPACE
#define IPlugin_iid "org.plugin.ImageProcessing.Segmentation.IPlugin"
Q_DECLARE_INTERFACE(IPlugin, IPlugin_iid)
QT_END_NAMESPACE

#endif
