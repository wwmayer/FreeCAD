// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2025 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#ifndef PARTGUI_PROGRESSINDICATOR_H
#define PARTGUI_PROGRESSINDICATOR_H

#include <Message_ProgressIndicator.hxx>
#include <Standard_Version.hxx>

#include <Mod/Part/PartGlobal.h>

class QProgressDialog;
class QWidget;

namespace PartGui {

class PartGuiExport ProgressIndicator : public Message_ProgressIndicator
{
public:
    ProgressIndicator (const QString &title, QWidget* parent = nullptr);
    ~ProgressIndicator () override;

    void Show (const Message_ProgressScope& theScope, const bool isForce) override;
    bool UserBreak() override;
    void Reset() override;

private:
    int steps {0};
    bool canceled {false};
    QProgressDialog* myProgress;
};

}

#endif // PARTGUI_PROGRESSINDICATOR_H
