/***************************************************************************
 *   Copyright (c) 2025 Andrew Shkolik <shkolik@gmail.com>                 *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QAction>
#include <QMenu>
#endif

#include "ViewProviderGordonSurface.h"
#include "TaskGordonSurface.h"
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>
#include <Mod/Surface/App/Gordon/FeatureGordonSurface.h>

PROPERTY_SOURCE(SurfaceGui::ViewProviderGordonSurface, PartGui::ViewProviderSpline)

namespace SurfaceGui
{

void ViewProviderGordonSurface::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act = menu->addAction(QObject::tr("Edit Gordon surface"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
    PartGui::ViewProviderSpline::setupContextMenu(menu, receiver, member);
}

bool ViewProviderGordonSurface::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        // When double-clicking on the item for this sketch the
        // object unsets and sets its edit mode without closing
        // the task panel

        Surface::GordonSurface* obj = this->getObject<Surface::GordonSurface>();

        Gui::TaskView::TaskDialog* dlg = Gui::Control().activeDialog();

        // start the edit dialog
        if (dlg) {
            TaskGordonSurface* tDlg = qobject_cast<TaskGordonSurface*>(dlg);
            if (tDlg) {
                tDlg->setEditedObject(obj);
            }
            Gui::Control().showDialog(dlg);
        }
        else {
            Gui::Control().showDialog(new TaskGordonSurface(this, obj));
        }
        return true;
    }
    else {
        return ViewProviderSpline::setEdit(ModNum);
    }
}

void ViewProviderGordonSurface::unsetEdit(int ModNum)
{
    PartGui::ViewProviderSpline::unsetEdit(ModNum);
}

QIcon ViewProviderGordonSurface::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Surface_GordonSurface");
}

void ViewProviderGordonSurface::highlightReferences(const References& refs, bool on)
{
    for (const auto& it : refs) {
        Part::Feature* base = dynamic_cast<Part::Feature*>(it.first);
        if (base) {
            PartGui::ViewProviderPartExt* svp = dynamic_cast<PartGui::ViewProviderPartExt*>(
                Gui::Application::Instance->getViewProvider(base));
            if (svp) {
                if (on) {
                    std::vector<Base::Color> colors;
                    TopTools_IndexedMapOfShape eMap;
                    TopExp::MapShapes(base->Shape.getValue(), TopAbs_EDGE, eMap);
                    colors.resize(eMap.Extent(), svp->LineColor.getValue());

                    for (const auto& jt : it.second) {
                        std::size_t idx = static_cast<std::size_t>(std::stoi(jt.substr(4)) - 1);
                        // check again that the index is in range because it's possible that
                        // the sub-names are invalid
                        if (idx < colors.size()) {
                            colors[idx] = Base::Color(1.0, 0.0, 1.0);  // magenta
                        }
                    }

                    svp->setHighlightedEdges(colors);
                }
                else {
                    svp->unsetHighlightedEdges();
                }
            }
        }
    }
}

}  // namespace SurfaceGui
