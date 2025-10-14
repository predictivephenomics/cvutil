/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: RoiManager.cpp

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

#include "include/RoiManager.h"

using namespace std;

//extern class GraphicsView;

RoiManager* RoiManager::GetInstance()
{
    static RoiManager instance;
    return &instance;
}

//void RoiManager::setScene(GraphicsScene *scene)
//{
//    gscene = scene;
//}

int RoiManager::getROICount()
{
    return int(rois.size());
}

int RoiManager::getSelectedROICount()
{
    int result = 0;

    for (auto r : rois)
        if (r != nullptr && r->isSelected())
            result++;

    return result;
}

vector<QGraphicsRectItem *> RoiManager::getROIs()
{
    vector<QGraphicsRectItem *> result;

    for (QGraphicsRectItem *r : rois)
        if (r != nullptr)
            result.push_back(r);
    
    return result;
}

QGraphicsRectItem * RoiManager::getROI(int idx)
{
    return rois[idx];
}

vector<QGraphicsRectItem *> RoiManager::getSelectedROIs()
{
    vector<QGraphicsRectItem *> result;

    for (auto r : rois)
        if (r != nullptr && r->isSelected())
            result.push_back(r);

    return result;
}

vector<int> RoiManager::getSelectedIndices()
{
    vector<int> result;

    for (int r = 0; r < rois.size(); r++)
        if (rois[r] != nullptr && rois[r]->isSelected())
            result.push_back(r);

    return result;
}

void RoiManager::moveSelected(int xp, int yp)
{
    for (auto r : rois)
    {
        if (r != nullptr && r->isSelected())
        {
            QRectF br = r->rect();
            br.setX(br.x() + xp);
            br.setY(br.y() + yp);
            r->setRect(br);
        }
    }

    emit selectedROIDimensionChanged();
}

//void RoiManager::deleteROI(int idx)
//{
//    //static int nullcount = 0;
//
//    if (idx < 0 || idx >= rois.size())
//    {
//        qCritical() << "Region-of-Interest index out-of-range.";
//        return;
//    }
//
//    if (rois[idx] == nullptr)
//        return;
//
//    gscene->removeItem(rois[idx]);
//    delete rois[idx];
//    rois[idx] = nullptr;
//    //nullcount++;
//
//    /*if (nullcount == 10)
//    {*/
//        vector<QGraphicsRectItem *> nwrois;
//
//        for (auto r : rois)
//        {
//            if (r)
//                nwrois.push_back(r);
//        }
//
//        rois = nwrois;
//        //nullcount = 0;
//    //}
//
//    // Remove roi name.
//    vector<string> nrois;
//    for (int i = 0; i < roinames.size(); i++)
//    {
//        if (i != idx)
//            nrois.push_back(roinames[i]);
//    }
//
//    roinames = nrois;
//
//    // Remove class name.
//    vector<string> crois;
//    for (int i = 0; i < classes.size(); i++)
//    {
//        if (i != idx)
//            crois.push_back(classes[i]);
//    }
//
//    classes = crois;
//    emit selectedROIDimensionChanged();
//}
//
//void RoiManager::deleteROI(string name)
//{
//    int deleteidx = -1;
//
//    for (int i = 0; i < roinames.size(); i++)
//        if (roinames[i] == name)
//            deleteidx = i;
//
//    if (deleteidx == -1)
//    {
//        qCritical() << "Region-of-Interest name not found.";
//        return;
//    }
//
//    deleteROI(deleteidx);
//}

void RoiManager::selectROI(int idx)
{
    rois[idx]->setSelected(true);
    emit selectedROIDimensionChanged();
}

void RoiManager::deleteAll()
{
    int nrois = int(rois.size());

    for (int idx = 0; idx < nrois; idx++)
    {
        //gscene->removeItem(rois[idx]);
        delete rois[idx];
        rois[idx] = nullptr;
    }

    rois.clear();
    roinames.clear();
    classes.clear();

    emit selectedROIDimensionChanged();
}

void RoiManager::deleteSelected()
{
    int nrois = int(rois.size());
    vector<QString> rnames, cnames;
    vector<QGraphicsRectItem *> ri;

    for (int idx = 0; idx < nrois; idx++)
    {
        if (rois[idx] != nullptr && rois[idx]->isSelected())
        {
            //gscene->removeItem(rois[idx]);
            delete rois[idx];
            rois[idx] = nullptr;
        }
        else if (rois[idx] != nullptr)
        {
            ri.push_back(rois[idx]);
            rnames.push_back(roinames[idx]);
            cnames.push_back(classes[idx]);
        }
    }

    rois = ri;
    roinames = rnames;
    classes = cnames;

    emit selectedROIDimensionChanged();
}

void RoiManager::clearSelection()
{
    int nrois = int(rois.size());
    
    for (int idx = 0; idx < nrois; idx++)
        if (rois[idx] != nullptr && rois[idx]->isSelected())
            rois[idx]->setSelected(false);
    
    //gscene->clearSelection();
    //selectedrois.clear();
    emit selectedROIDimensionChanged();
}

void RoiManager::clearSelection(int idx)
{
    rois[idx]->setSelected(false);
    emit selectedROIDimensionChanged();
}

void RoiManager::selectAll()
{
    for (auto roi : rois)
        roi->setSelected(true);

    emit selectedROIDimensionChanged();
}

void RoiManager::setROIBorderWidth(double s)
{
    //penwidth = int(1.0 / s);
    penwidth = int(round((log(1.0 / s) - log(0.005)) / log(2) / 4.0));
    penwidth = (penwidth == 0) ? 1 : penwidth;

    zoomscale = s;

    for (QGraphicsRectItem *roi : rois)
    {
        if (roi == nullptr)
            continue;

        QPen p = roi->pen();
        p.setColor(roicolor);
        p.setWidth(penwidth);
        roi->setPen(p);
    }
}

QGraphicsRectItem * RoiManager::createROI(int x1, int y1, int x2, int y2, qreal w, qreal h, bool update)
{
    QGraphicsRectItem *result = nullptr;

    if (!IsROICreating)
    {
        if (x1 < 0 || x1 >= w) return result;
        if (y1 < 0 || y1 >= h) return result;
        if (x2 < 0 || x2 >= w) return result;
        if (y2 < 0 || y2 >= h) return result;
    }
    else
    {
        x1 = MIN(MAX(0, x1), w);
        y1 = MIN(MAX(0, y1), h);
        x2 = MIN(MAX(0, x2), w);
        y2 = MIN(MAX(0, y2), h);
    }

    int xpos = MIN(x1, x2), ypos = MIN(y1, y2);
    int xsize = abs(x2 - x1), ysize = abs(y2 - y1);

    //cout << "State : update = " << update << " - IsROICreating = " << IsROICreating;

    if ((!update) && (!IsROICreating))
    {
        clearSelection();

        QGraphicsRectItem *item = new QGraphicsRectItem(xpos, ypos, xsize, ysize);
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, true);
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, true);
        
        rois.push_back(item);
        classes.push_back(defaultclassname);
        roinames.push_back(roinameprefix + QString::number(roicount++));

        int idx = int(rois.size()) - 1;

        QPen p = rois[idx]->pen();
        p.setColor(roicolor);
        p.setWidth(penwidth);
        rois[idx]->setPen(p);

        /*gscene->addItem(rois[idx]);
        rois[idx]->setSelected(true);*/
        IsROICreating = true;
        result = rois[idx];
        //emit selectedROIDimensionChanged();
        //cout << "IsROICreating = true\n";
    }
    else if ((!update) && (IsROICreating))
    {
        IsROICreating = false;
        //cout << "IsROICreating = false\n";
    }
    else if (update && IsROICreating)
    {
        int idx = int(rois.size()) - 1;
        rois[idx]->setRect(xpos, ypos, xsize, ysize);
        emit selectedROIDimensionChanged();
        //cout << "IsROICreating = continuing\n";
    }

    return result;
}

vector<QGraphicsRectItem *> RoiManager::copypaste(vector<int> copyrois, vector<QRectF> copyroirects, qreal w, qreal h)
{
    bool docopypaste = true;
    vector<QGraphicsRectItem *> result;

    for (int i = 0; i < copyrois.size(); i++)
    {
        if (rois.size() == 0 || (rois.size() - 1) < copyrois[i] || rois[copyrois[i]] == nullptr)
        {
            docopypaste = false;
            break;
        }
        else if (rois[copyrois[i]]->rect() != copyroirects[i])
        {
            docopypaste = false;
            break;
        }
    }
    
    if (!docopypaste)
        return result;

    //qreal w = gscene->width(), h = gscene->height();
    int x1 = 0, y1 = 0, x2 = 100.0 / zoomscale, y2 = 100.0 / zoomscale;
    int minx = INT_MAX, maxx = 0, miny = INT_MAX, maxy = 0;
    int xpos, ypos;

    int xdiff = (x2 - x1);
    int ydiff = (y2 - y1);
    
    clearSelection();

    for (int i = 0; i < copyroirects.size(); i++)
    {
        xpos = ((copyroirects[i].x() + xdiff) < 0) ? 0 :
            ((((copyroirects[i].x() + copyroirects[i].width()) + xdiff) > w) ? (w - copyroirects[i].width()) :
            (copyroirects[i].x() + xdiff));
        ypos = ((copyroirects[i].y() + ydiff) < 0) ? 0 :
            ((((copyroirects[i].y() + copyroirects[i].height()) + ydiff) > h) ? (h - copyroirects[i].height()) :
            (copyroirects[i].y() + ydiff));

        QGraphicsRectItem *item = new QGraphicsRectItem(xpos, ypos, copyroirects[i].width(), copyroirects[i].height());
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, true);
        item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, true);

        rois.push_back(item);
        roinames.push_back(roinameprefix + QString::number(roicount++));
        classes.push_back(classes[copyrois[i]]);

        int idx = int(rois.size()) - 1;

        QPen p = rois[idx]->pen();
        p.setColor(roicolor);
        p.setWidth(penwidth);
        rois[idx]->setPen(p);

        result.push_back(rois[idx]);
        /*gscene->addItem(rois[idx]);
        rois[idx]->setSelected(true);*/
    }

    //emit selectedROIDimensionChanged();
    return result;
}

void RoiManager::moveROIs(int x1, int y1, int x2, int y2, qreal w, qreal h)
{
    //qreal w = gscene->width(), h = gscene->height();
    
    x1 = MIN(MAX(0, x1), w);
    y1 = MIN(MAX(0, y1), h);
    x2 = MIN(MAX(0, x2), w);
    y2 = MIN(MAX(0, y2), h);
    
    int xdiff = (x2 - x1);
    int ydiff = (y2 - y1);
    
    int minx = INT_MAX, maxx = 0, miny = INT_MAX, maxy = 0;

    // Get minimum bounding rectangle of selected ROIs.
    for (auto &roi : rois)
    {
        if (roi != nullptr && roi->isSelected())
        {
            QRectF r = roi->rect();

            minx = MIN(r.x(), minx);
            maxx = MAX(r.x() + r.width(), maxx);
            miny = MIN(r.y(), miny);
            maxy = MAX(r.y() + r.height(), maxy);
        }
    }

    if (minx == INT_MAX)
        return;

    int xpos = ((minx + xdiff) < 0) ? 0 : (((maxx + xdiff) > w) ? (minx - maxx + w) : (minx + xdiff));
    int ypos = ((miny + ydiff) < 0) ? 0 : (((maxy + ydiff) > h) ? (miny - maxy + h) : (miny + ydiff));
    
    xdiff = xpos - minx;
    ydiff = ypos - miny;
    
    // Set the new positions
    for (auto &roi : rois)
    {
        if (roi->isSelected())
        {
            QRectF r = roi->rect();
            roi->setRect(r.x() + xdiff, r.y() + ydiff, r.width(), r.height());
        }
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::resizeROIs(BorderHoverMode m, QRectF hoverrect, vector<QRectF> srois, int x1, int y1, int x2, int y2, qreal w, qreal h)
{
    //qreal w = gscene->width(), h = gscene->height();

    x1 = MIN(MAX(0, x1), w);
    y1 = MIN(MAX(0, y1), h);
    x2 = MIN(MAX(0, x2), w);
    y2 = MIN(MAX(0, y2), h);
    
    int xdiff = (x2 - x1);
    int ydiff = (y2 - y1);
    
    //auto selectedrois = getSelectedROIs();
    auto selectedindices = getSelectedIndices();
    qreal xmultiplier = 0.0, ymultiplier = 0.0;

    int hw = hoverrect.width();
    int hh = hoverrect.height();
    
    qreal xfinal = 0, yfinal = 0, wfinal = 0, hfinal = 0;

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        xmultiplier = srois[i].width() / hw;
        ymultiplier = srois[i].height() / hh;
        
        switch (m)
        {
        case BorderHoverMode::TopLeft:
            xfinal = sr.left() + xdiff * xmultiplier;
            yfinal = sr.top() + ydiff * ymultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setTopLeft({ xfinal, yfinal });
            break;
        case BorderHoverMode::Top:
            yfinal = sr.top() + ydiff * ymultiplier;
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setTop(yfinal);
            break;
        case BorderHoverMode::TopRight:
            xfinal = sr.right() + xdiff * xmultiplier;
            yfinal = sr.top() + ydiff * ymultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setTopRight({ xfinal, yfinal });
            break;
        case BorderHoverMode::Right:
            xfinal = sr.right() + xdiff * xmultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);

            sr.setRight(xfinal);
            break;
        case BorderHoverMode::BottomRight:
            xfinal = sr.right() + xdiff * xmultiplier;
            yfinal = sr.bottom() + ydiff * ymultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setBottomRight({ xfinal, yfinal });
            break;
        case BorderHoverMode::Bottom:
            yfinal = sr.bottom() + ydiff * ymultiplier;
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setBottom(yfinal);
            break;
        case BorderHoverMode::BottomLeft:
            xfinal = sr.left() + xdiff * xmultiplier;
            yfinal = sr.bottom() + ydiff * ymultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);
            yfinal = (yfinal < 0) ? 0 : ((yfinal > h) ? h : yfinal);

            sr.setBottomLeft({ xfinal, yfinal });
            break;
        case BorderHoverMode::Left:
            xfinal = sr.left() + xdiff * xmultiplier;
            xfinal = (xfinal < 0) ? 0 : ((xfinal > w) ? w : xfinal);

            sr.setLeft(xfinal);
            break;
        }

        if (sr.width() >= 0 && sr.height() >= 0)
            rois[selectedindices[i]]->setRect(sr);
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::deleteList(vector<int> indices)
{
    int nrois = int(rois.size());
    vector<QString> rnames, cnames;
    vector<QGraphicsRectItem *> ri;

    if (indices.size() == 0)
        return;

    for (int idx = 0; idx < indices.size(); idx++)
    {
        if (idx >= 0 && idx < nrois && rois[idx] != nullptr)
        {
            //gscene->removeItem(rois[idx]);
            delete rois[idx];
            rois[idx] = nullptr;
        }
        else if (rois[idx] != nullptr)
        {
            ri.push_back(rois[idx]);
            rnames.push_back(roinames[idx]);
            cnames.push_back(classes[idx]);
        }
    }

    rois = ri;
    roinames = rnames;
    classes = cnames;

    emit selectedROIDimensionChanged();
}

void RoiManager::setX(int x, qreal scenewidth)
{
    qreal w = scenewidth;
    auto selectedindices = getSelectedIndices();
    int xi = 0, mr = 0;
    
    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        mr = MAX(mr, sr.width());
    }

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        xi = MIN(MAX(0, x), w - mr);
        sr.moveLeft(xi);
        rois[selectedindices[i]]->setRect(sr);
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::setY(int y, qreal sceneheight)
{
    qreal h = sceneheight;
    auto selectedindices = getSelectedIndices();
    int yi = 0, mb = 0;

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        mb = MAX(mb, sr.height());
    }

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        yi = MIN(MAX(0, y), h - mb);
        sr.moveTop(yi);
        rois[selectedindices[i]]->setRect(sr);
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::setWidth(int width, qreal scenewidth)
{
    qreal w = scenewidth;
    auto selectedindices = getSelectedIndices();
    int wi = 0;

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        wi = MIN(w - sr.x(), width);
        sr.setWidth(wi);
        rois[selectedindices[i]]->setRect(sr);
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::setHeight(int height, qreal sceneheight)
{
    qreal h = sceneheight;
    auto selectedindices = getSelectedIndices();
    int hi = 0;

    for (int i = 0; i < selectedindices.size(); i++)
    {
        QRectF sr = rois[selectedindices[i]]->rect();
        hi = MIN(h - sr.y(), height);
        sr.setHeight(hi);
        rois[selectedindices[i]]->setRect(sr);
    }

    emit selectedROIDimensionChanged();
}

void RoiManager::setSelectable(bool on)
{
    for (auto &roi : rois)
    {
        roi->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, on);
        roi->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, on);
    }
}

BorderHoverMode RoiManager::hoverMode(int idx, qreal x, qreal y)
{
    QRectF sbr = rois[idx]->rect();
    qreal multiplier = 32;
    QRectF temp1(sbr.x() + multiplier * penwidth, sbr.y() + multiplier * penwidth,
        sbr.width() - 2 * multiplier * penwidth, sbr.height() - 2 * multiplier * penwidth);
    QRectF temp2(sbr.x() - multiplier * penwidth, sbr.y() - multiplier * penwidth,
        sbr.width() + 2 * multiplier * penwidth, sbr.height() + 2 * multiplier * penwidth);

    if (x > temp2.x() && x <= temp1.x())
    {
        if (y > temp2.y() && y <= temp1.y())
            return BorderHoverMode::TopLeft;
        else if (y > temp1.y() && y <= (temp1.y() + temp1.height()))
            return BorderHoverMode::Left;
        else if (y > (temp1.y() + temp1.height()) && y <= (temp2.y() + temp2.height()))
            return BorderHoverMode::BottomLeft;
    }
    else if (x > temp1.x() && x <= (temp1.x() + temp1.width()))
    {
        if (y > temp2.y() && y <= temp1.y())
            return BorderHoverMode::Top;
        else if (y > (temp1.y() + temp1.height()) && y <= (temp2.y() + temp2.height()))
            return BorderHoverMode::Bottom;
    }
    else if (x > (temp1.x() + temp1.width()) && x <= (temp2.x() + temp2.width()))
    {
        if (y > temp2.y() && y <= temp1.y())
            return BorderHoverMode::TopRight;
        else if (y > temp1.y() && y <= (temp1.y() + temp1.height()))
            return BorderHoverMode::Right;
        else if (y > (temp1.y() + temp1.height()) && y <= (temp2.y() + temp2.height()))
            return BorderHoverMode::BottomRight;
    }

    return BorderHoverMode::None;
}

void RoiManager::updateBoundaries(qreal w, qreal h)
{
    //qreal w = gscene->width(), h = gscene->height();
    bool updated = false;

    vector<int> dlist;
    QRectF r;

    for (int i = 0; i < rois.size(); i++)
    {
        r = rois[i]->rect();

        if (r.width() > w || r.height() > h)
        {
            qWarning().noquote().nospace() << "Deleting the region-of-interest '" << roinames[i] << "' as it exceeds the image boundaries.";
            dlist.push_back(i);
        }
        else
        {
            if (r.right() > w)
            {
                r.moveRight(w - r.right());
                updated = true;
            }
            else if (r.bottom() > h)
            {
                r.moveBottom(h - r.bottom());
                updated = true;
            }
        }
    }

    if (dlist.size() > 0)
        deleteList(dlist);

    if (updated)
        emit selectedROIDimensionChanged();
}

std::string RoiManager::getClassName(int idx)
{
    if (rois[idx] == nullptr)
        throw "Unknown region-of-interest";

    return classes[idx].toStdString();
}

void RoiManager::setClassName(int idx, std::string cname)
{
    if (rois[idx] == nullptr)
        throw "Unknown region-of-interest";

    classes[idx] = QString::fromStdString(cname);
}

void RoiManager::setDefaultClassName(std::string cname)
{
    defaultclassname = QString::fromStdString(cname);
}

std::string RoiManager::getROIName(int idx)
{
    if (rois[idx] == nullptr)
        throw "Unknown region-of-interest";

    return roinames[idx].toStdString();
}

void RoiManager::setROIName(int idx, std::string cname)
{
    if (rois[idx] == nullptr)
        throw "Unknown region-of-interest";

    roinames[idx] = QString::fromStdString(cname);
}

void RoiManager::selectROIbyName(std::string name)
{
    ptrdiff_t pos = find(roinames.begin(), roinames.end(), QString::fromStdString(name)) - roinames.begin();
    rois[pos]->setSelected(true);
    emit selectedROIDimensionChanged();
}

std::vector<QGraphicsRectItem *> RoiManager::loadAnnotation(QString filepath)
{
    //ifstream input(filepath);
    QFile input(filepath);
    vector<QGraphicsRectItem *> result;

    /*function<void(string &)> trim = [](string &s) 
    {
        s.erase(0, s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1).find_first_not_of(" \t\n\r\f\v"));
    };*/

    clearSelection();

    // The format of csv should be as follows:
    // #ROI Name, Class Name, X Position, Y Position, Width, Height
    // roi_1, class_1, 80, 70, 200, 150  # Roi containing sample object
    //
    // Hence, the annotations contain the list of all the ROIs in a text
    // file in CSV format. Any text starting with # is considered as 
    // comment till the end of the line.
    if (input.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line, line2, trln, val;
        int lineno = 0;
        QTextStream in(&input);

        //while (getline(input, line))
        while (!in.atEnd())
        {
            line = in.readLine();
            
            lineno++;
            line2 = line;
            //if (line.find_first_of("#") != string::npos)
            if (line.indexOf("#") != -1)
                //trln = line.erase(line.find_first_of("#"));
                trln = line.left(line.indexOf("#"));
            else
                trln = line;
            //trim(trln);
            trln = trln.trimmed();
            
            if (trln.length() == 0)
                continue;

            QStringList vals = trln.split(",");
            //stringstream ss(trln);
            bool ignore = false;

            //while (getline(ss, val, ','))
            for (int i = 0; i < vals.size(); i++)
            {
                //trim(val);
                vals[i] = vals[i].trimmed();
                //vals.push_back(val);

                if (vals.size() > 2 && vals[i].length() == 0)
                    ignore = true;
            }

            if (vals.size() != 6 || ignore)
            {
                qWarning().noquote().nospace() << "Unknown ROI dimensions (At line number " << lineno << "): " << line2;
                continue;
            }

            QGraphicsRectItem *item = new QGraphicsRectItem(vals[2].toInt(), vals[3].toInt(), vals[4].toInt(), vals[5].toInt());
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, true);
            item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, true);

            rois.push_back(item);
            roinames.push_back(vals[0]);
            classes.push_back(vals[1]);

            int idx = int(rois.size()) - 1;

            QPen p = rois[idx]->pen();
            p.setColor(roicolor);
            p.setWidth(penwidth);
            rois[idx]->setPen(p);

            //gscene->addItem(rois[idx]);
            result.push_back(rois[idx]);
        }

        input.close();
        return result;
    }
    else
        qCritical() << "(Error code = " << strerror(errno) << "): Cannot open file for reading.";

    return result;
}

void RoiManager::saveAnnotation(QString filepath)
{
    int nonnullcount = 0;

    for (int i = 0; i < rois.size(); i++)
        if (rois[i] != nullptr)
            nonnullcount++;

    if (nonnullcount == 0)
    {
        qWarning() << "Region-of-Interests not found.";
        return;
    }

    QFile output(filepath);
    //ofstream output(filepath);

    if (output.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&output);
        //out.setGenerateByteOrderMark(true);
        out << "#ROI Name, Class Name, X Position, Y Position, Width, Height\n";

        for (int i = 0; i < rois.size(); i++)
        {
            if (rois[i] != nullptr)
            {
                out << roinames[i] << ", " << classes[i] << 
                    ", " << rois[i]->rect().x() << 
                    ", " << rois[i]->rect().y() <<
                    ", " << rois[i]->rect().width() <<
                    ", " << rois[i]->rect().height() << "\n";
            }
        }

        output.close();
    }
    else
        qCritical() << "(Error code = " << strerror(errno) << "): Cannot open file for reading.";
}

void RoiManager::setMessageHandler(QtMessageHandler logger)
{
    qInstallMessageHandler(logger);
}
