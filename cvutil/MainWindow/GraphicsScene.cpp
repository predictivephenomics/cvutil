/*  Copyright (C) 2018-2020 Noble Research Institute, LLC

File: GraphicsScene.cpp

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

//#include <ImageProcessor.h>
#include <RoiManager.h>
#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(QObject* parent) : QGraphicsScene(parent)
{
    //RoiManager *mgr = RoiManager::GetInstance();
    
    //QFont f("Arial", 15, QFont::Bold);
    
    //this->addText("Load a plant root image from File menu\n             or drag and drop it here.", f);

    //connect(this, &GraphicsScene::selectionChanged, mgr, &RoiManager::updateSelections);
}

void GraphicsScene::setMouseMode(MouseMode _mode)
{
    mode = _mode;
    this->clearSelection();
    RoiManager *mgr = RoiManager::GetInstance();
    emit mgr->selectedROIDimensionChanged();

    if (mode == MouseMode::ImagePanning || mode == MouseMode::PixelValue)
        mgr->setSelectable(false);
    else
        mgr->setSelectable(true);
}

void GraphicsScene::setEnabledTransformations(bool on)
{
    enabletransformations = on;
}

void GraphicsScene::keyPressEvent(QKeyEvent *event)
{
    RoiManager *mgr = RoiManager::GetInstance();

    if (event->key() == Qt::Key_Delete)
    {
        if (pos != QPointF(-1.0, -1.0))
        {
            QGraphicsScene::keyPressEvent(event);
            return;
        }

        if (mgr->getSelectedROICount() == 0)
        {
            QGraphicsScene::keyPressEvent(event);
            return;
        }

        auto items = mgr->getSelectedROIs();
        for (int idx = 0; idx < items.size(); idx++)
            removeItem(items[idx]);
        mgr->deleteSelected();
    }
    else if (event->key() == Qt::Key_A && event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        mgr->selectAll();
    }
    else if (event->key() == Qt::Key_C && event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        copyrois = mgr->getSelectedIndices();
        auto selrois = mgr->getSelectedROIs();
        copyroirects.clear();

        for (auto& selroi : selrois)
            copyroirects.push_back(selroi->rect());
    }
    else if (event->key() == Qt::Key_V && event->modifiers() == Qt::KeyboardModifier::ControlModifier)
    {
        if (copyrois.size() > 0)
        {
            auto items = mgr->copypaste(copyrois, copyroirects, width(), height());

            if (items.size() == 0)
            {
                copyrois.clear();
                copyroirects.clear();
            }
            else
            {
                for (int i = 0; i < items.size(); i++)
                {
                    addItem(items[i]);
                    items[i]->setSelected(true);
                }

                emit mgr->selectedROIDimensionChanged();
            }
        }
        else
        {
            copyrois.clear();
            copyroirects.clear();
        }
    }
    else
    {
        QGraphicsScene::keyPressEvent(event);
        return;
    }
}

//void GraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent * event)
//{
//    event->acceptProposedAction();
//}
//
//void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent * event)
//{
//    event->acceptProposedAction();
//}
//
//void GraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->acceptProposedAction();
//}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    RoiManager *mgr = RoiManager::GetInstance();
    
    if (mode == MouseMode::PixelValue)
    {
        pos = event->scenePos();
        QGraphicsScene::mousePressEvent(event);
    }
    else if (mode == MouseMode::ImagePanning)
    {
        QGraphicsScene::mousePressEvent(event);
    }
    else if (mode == MouseMode::RegionOfInterest)
    {
        if (!enabletransformations)
            return;

        if (submode == MouseSubMode::MouseMove)
        {
            QGraphicsScene::mousePressEvent(event);
            if (mgr->getSelectedROICount() == 0)
            {
                qreal w = width(), h = height();

                QPointF _pos = event->scenePos();

                if (_pos.x() >= 0 && _pos.x() <= w && _pos.y() >= 0 && _pos.y() <= h)
                    pos = _pos;

                // pos = event->scenePos();
            }
            else
                movepos = event->scenePos();
        }
        else
        {
            resizepos = event->scenePos();

            auto sr = mgr->getSelectedROIs();
            //QGraphicsScene::mousePressEvent(event);
        }
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mode == MouseMode::PixelValue)
    {
        QPointF pos2 = event->scenePos();

        if (pos.toPoint() == pos2.toPoint() && pos != QPointF(-1.0, -1.0))
        {
            int _x = int(floor(pos.x()));
            int _y = int(floor(pos.y()));

            pos = QPointF(-1.0, -1.0);
            emit showPixelInfo(_x, _y);
        }

        QGraphicsScene::mouseReleaseEvent(event);
    }
    else if (mode == MouseMode::ImagePanning)
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
    else if (mode == MouseMode::RegionOfInterest)
    {
        RoiManager *mgr = RoiManager::GetInstance();
        if (pos != QPointF(-1.0, -1.0))
        {
            mgr->createROI(-1, -1, -1, -1, width(), height(), false);
            pos = QPointF(-1.0, -1.0);
        }

        movepos = QPointF(-1.0, -1.0);
        resizepos = QPointF(-1.0, -1.0);
        /*hoveridx = -1;
        hoverrect = QRectF(0.0, 0.0, 0.0, 0.0);*/
        
        //if (mgr->getSelectedIndices().size() > 0)
            //srois.clear();

        //QGraphicsSceneMouseEvent *ev = static_cast<QGraphicsSceneMouseEvent *>(event);
        //QPointF pos2 = ev->scenePos();
        

        // If the coordinates of mouse at the time of
        // pressing (start position) is not equal to 
        // the the coordinates of mouse at the time 
        // of releasing, then we just reset the start 
        // position to QPointF(-1.0, -1.0), so we 
        // disable mouse move updates.

        // Then we need to move all the selected rois accordingly.
        //if (pos.toPoint() != pos2.toPoint())
        //{
        //    pos = QPointF(-1.0, -1.0);
        //    //emit showPixelInfo(_x, _y);
        //}
        //else
        //{
        //    // If the coordinates of mouse at the time of
        //    // pressing (start position) is not equal to 
        //    // the the coordinates of mouse at the time 
        //    // of releasing, then we need to toggle the 
        //    // selection of the roi on which the mouse is 
        //    // located. If there is no roi under the mouse,
        //    // we do nothing.
        //}

        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    RoiManager *mgr = RoiManager::GetInstance();

    if (mode == MouseMode::PixelValue)
    {
        pos = QPointF(-1.0, -1.0);
        if (mgr->getSelectedROICount() == 0)
            QGraphicsScene::mouseMoveEvent(event);
    }
    else if (mode == MouseMode::ImagePanning)
    {
        if (mgr->getSelectedROICount() == 0)
            QGraphicsScene::mouseMoveEvent(event);
    }
    else if (mode == MouseMode::RegionOfInterest)
    {
        if (!enabletransformations)
            return;

        QGraphicsSceneMouseEvent *ev = static_cast<QGraphicsSceneMouseEvent *>(event);
        QPointF pos2 = ev->scenePos();
        
        if (pos == QPointF(-1.0, -1.0))
        {
            if (mgr->getSelectedROICount() == 0)
            {
                if (bmode != BorderHoverMode::None)
                {
                    bmode = BorderHoverMode::None;
                    submode = MouseSubMode::MouseMove;
                    QCursor sh(Qt::CrossCursor);
                    views()[0]->viewport()->setCursor(sh);
                }

                QGraphicsScene::mouseMoveEvent(event);
                return;
            }

            if (movepos != QPointF(-1.0, -1.0))
            {
                // Handle movement of ROIs
                int mxstart = int(floor(movepos.x()));
                int mystart = int(floor(movepos.y()));
                int mxend = int(floor(pos2.x()));
                int myend = int(floor(pos2.y()));

                mgr->moveROIs(mxstart, mystart, mxend, myend, width(), height());
                movepos = pos2;
                return;
            }
            
            if (resizepos != QPointF(-1.0, -1.0))
            {
                // To resize ROIs
                int mxstart = int(floor(resizepos.x()));
                int mystart = int(floor(resizepos.y()));
                int mxend = int(floor(pos2.x()));
                int myend = int(floor(pos2.y()));

                mgr->resizeROIs(bmode, hoverrect, srois, mxstart, mystart, mxend, myend, width(), height());
                resizepos = pos2;
                return;
            }
            else
            {
                // Check if mouse cursor is on the border of any of the selected
                // rois. Then change the mouse sub mode to 'resize'. Also change 
                // the mouse cursor to resize cursors.
                // Else change the mouse sub mode to 'move'. Also change the mouse 
                // cursor to move cursor.
                hoveridx = -1;
                hoverrect = QRectF(0.0, 0.0, 0.0, 0.0);
                srois.clear();
                for (auto id : mgr->getSelectedIndices())
                {
                    bmode = mgr->hoverMode(id, pos2.x(), pos2.y());
                    if (bmode != BorderHoverMode::None)
                    {
                        hoveridx = id;
                        hoverrect = mgr->getROI(id)->rect();
                        auto selrois = mgr->getSelectedROIs();
                        srois.clear();

                        for (auto& selroi : selrois)
                            srois.push_back(selroi->rect());
                        break;
                    }
                }

                if (bmode != BorderHoverMode::None)
                {
                    submode = MouseSubMode::MouseResize;
                    QCursor sh;

                    if (rotation == 0 || rotation == 180)
                    {
                        if (bmode == BorderHoverMode::Top || bmode == BorderHoverMode::Bottom)
                            sh = QCursor(Qt::CursorShape::SizeVerCursor);
                        else if (bmode == BorderHoverMode::Left || bmode == BorderHoverMode::Right)
                            sh = QCursor(Qt::CursorShape::SizeHorCursor);
                        else if (bmode == BorderHoverMode::TopLeft || bmode == BorderHoverMode::BottomRight)
                            sh = QCursor(Qt::CursorShape::SizeFDiagCursor);
                        else
                            sh = QCursor(Qt::CursorShape::SizeBDiagCursor);
                    }
                    else //if (rotation == 90 || rotation == 270)
                    {
                        if (bmode == BorderHoverMode::Top || bmode == BorderHoverMode::Bottom)
                            sh = QCursor(Qt::CursorShape::SizeHorCursor);
                        else if (bmode == BorderHoverMode::Left || bmode == BorderHoverMode::Right)
                            sh = QCursor(Qt::CursorShape::SizeVerCursor);
                        else if (bmode == BorderHoverMode::TopLeft || bmode == BorderHoverMode::BottomRight)
                            sh = QCursor(Qt::CursorShape::SizeBDiagCursor);
                        else
                            sh = QCursor(Qt::CursorShape::SizeFDiagCursor);
                    }

                    views()[0]->viewport()->setCursor(sh);
                }
                else if (bmode == BorderHoverMode::None && submode != MouseSubMode::MouseMove)
                {
                    submode = MouseSubMode::MouseMove;
                    QCursor sh(Qt::CrossCursor);
                    views()[0]->viewport()->setCursor(sh);
                }

                return;
            }
            
        }

        int xstart = int(floor(pos.x()));
        int ystart = int(floor(pos.y()));

        int xmove = int(floor(pos2.x()));
        int ymove = int(floor(pos2.y()));

        int rectidx = mgr->getROICount() - 1;
        QRectF lastrect;

        if (rectidx >= 0)
            lastrect = mgr->getROI(rectidx)->rect();

        if (!(xstart == xmove && ystart == ymove))
        {
            // We need to make a decision as to what to do 
            // when the mouse is moved while pressed. Should we
            // 1. increase the size of the roi we are currently creating?
            // 2. move an roi from one place to another?
            // 3. resize an roi after creation?
            // 4. about to create a new roi?
            if (rectidx < 0)
            {
                // we are about to create a new roi as there are no rois.
                auto item = mgr->createROI(xstart, ystart, xmove, ymove, width(), height(), false);
                addItem(item);
                item->setSelected(true);
                emit mgr->selectedROIDimensionChanged();
            }
            else
            {
                // When the coordinates of mouse at the time of
                // pressing is same as the position of last roi,
                // then we are still creating the roi by 
                // increasing/decreasing the size.
                //if (!(xstart == -1 && ystart == -1))
                if (xstart == lastrect.left() || xstart == lastrect.right())
                {
                    if (ystart == lastrect.top() || ystart == lastrect.bottom())
                        mgr->createROI(xstart, ystart, xmove, ymove, width(), height(), true);
                }
                else
                {
                    // From this point, we will process only already created rois
                    // which are selected.

                    // If the coordinates of mouse at the time of
                    // pressing is not the same as the position of last 
                    // roi, then we need few more conditions.

                    if (mgr->getSelectedROICount() > 0)
                    {
                        // Is the coordinates of mouse at the time of
                        // pressing inside one of the selected rois?
                        // Then we need to move all the selected rois accordingly.
                        QGraphicsScene::mouseMoveEvent(event);
                        return;
                        // Is the coordinates of mouse at the time of
                        // pressing on one of the borders of the selected rois?
                        // Then we need to resize the selected rois accordingly.
                    }
                    else
                    {
                        // If the coordinates of mouse at the time of
                        // pressing is out of the boundaries of the selected 
                        // rois, then we need to create a new roi.
                        auto item = mgr->createROI(xstart, ystart, xmove, ymove, width(), height(), false);
                        addItem(item);
                        item->setSelected(true);
                        emit mgr->selectedROIDimensionChanged();
                    }
                }
            }
        }
        else
        {
            QGraphicsScene::mouseMoveEvent(event);
            return;
        }
    }
}

void GraphicsScene::setViewRotation(int rot)
{
    rotation = rot;
}
