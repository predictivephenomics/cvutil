/*
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: PluginManager.cpp

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

#include "include/PluginManager.h"
#include "PluginUI/ParameterListWidget.h"

using namespace std;

void PluginManager::Load(string path)
{
    pluginsDir = QDir(QApplication::instance()->applicationDirPath());
    pluginsDir.cd("plugins");

    for (QString &fileName : pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        
        if (plugin)
        {
            IPlugin *p = qobject_cast<IPlugin *>(plugin);
            plugins.push_back(p);
            plugin_names.push_back(p->getName());
            qInfo().noquote().nospace() << "Plugin " << QString::fromStdString(p->getName()) << " loaded.";
            pluginfiles += fileName;
        }
    }
}

IPlugin *PluginManager::GetPluginByName(string plugin_name)
{
    for (int i = 0; i < plugins.size(); i++)
        if (plugins[i]->getName() == plugin_name)
            return plugins[i];

    return nullptr;
}

vector<IPlugin *> PluginManager::GetPlugins()
{
    return plugins;
}

vector<QWidget *> PluginManager::GetPluginUIs()
{
    if (plugins.size() == 0)
        return{};

    vector<QWidget *> result;

    for (auto plugin : plugins)
        result.push_back(new ParameterListWidget(plugin));

    return result;
}

vector<string> PluginManager::ListNames()
{
    return plugin_names;
}
