/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: PluginManager.h

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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

// To disable warnings from external headers.
#pragma warning(push, 0)
#include <QtWidgets/QtWidgets>
#include <QtWidgets/qlayout.h>
#pragma warning(pop)

#ifdef _WIN64
#if (!defined PLAPI)
#if (defined PLUGINMANAGER_SOURCE)
#define PLAPI __declspec(dllexport)
#else
#define PLAPI __declspec(dllimport)
#endif
#endif
#elif defined(__linux__) || defined(__APPLE__)
#if (!defined PLAPI)
#if (defined PLUGINMANAGER_SOURCE)
#define PLAPI __attribute__((visibility("default")))
#else
#define PLAPI
#endif
#endif
#endif

#include "PluginInterfaces.h"

#if (defined PLUGINMANAGER_SOURCE)
#include "../PluginUI/ParameterListWidget.h"
#endif

class PLAPI PluginManager
{
    PluginManager() {};

    QDir pluginsDir;
    QStringList pluginfiles;
    std::vector<std::string> plugin_names;
    std::vector<IPlugin*> plugins;

public:
    static PluginManager* GetInstance()
    {
        static PluginManager instance;
        return &instance;
    }

    // FIXME - The following is the correct 
    // functionality.

    // If the path is full file path, then only 
    // the plugin is loaded. If the the path is 
    // a directory, all the plugins in the path 
    // are loaded. If the path is empty, then the 
    // function will look for the 'plugins' folder
    // in the application directory.
    void Load(std::string path = "");
    
    // Return all plugins. 
    std::vector<IPlugin *> GetPlugins();

    // Get Plugin UIs. While the plugin objects may be single-instance objects,
    // the UIs can be multiple instanced, where each instance of multiple
    // instances binf to the same plugin.
    std::vector<QWidget *> GetPluginUIs();

    // Returns plugin after searching by name.
    IPlugin *GetPluginByName(std::string plugin_name);

    // List plugin names
    std::vector<std::string> ListNames();
};





#endif

