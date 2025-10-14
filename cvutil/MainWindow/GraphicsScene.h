/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: GraphicsScene.h

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

#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QtWidgets/QtWidgets>
#include <RoiManager.h>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT;

private:
    QPointF pos = QPointF(-1.0, -1.0);
    QPointF movepos = QPointF(-1.0, -1.0);

    // For resizing
    QPointF resizepos = QPointF(-1.0, -1.0);
    int hoveridx = -1;
    QRectF hoverrect = QRectF(0.0, 0.0, 0.0, 0.0);
    std::vector<QRectF> srois;

    // For copy and paste
    std::vector<int> copyrois;
    std::vector<QRectF> copyroirects;

    MouseMode mode = MouseMode::ImagePanning;
    
    enum class MouseSubMode { MouseMove, MouseResize };
    MouseSubMode submode = MouseSubMode::MouseMove;
    BorderHoverMode bmode = BorderHoverMode::None;

    bool enabletransformations = false;
    int rotation = 0;
public:
    GraphicsScene(QObject* parent = 0);

    MouseMode getMouseMode() const { return mode; }
    void setMouseMode(MouseMode _mode);

    void setEnabledTransformations(bool on);
    void setViewRotation(int rot);

signals:
    void showPixelInfo(int rowid, int colid);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    //void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    //void dropEvent(QGraphicsSceneDragDropEvent *event);
    //void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
};



#endif
