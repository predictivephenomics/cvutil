/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: GraphicsScene.h

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
