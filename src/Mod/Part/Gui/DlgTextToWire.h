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

#ifndef PARTGUI_DLGTEXTTOWIRE_H
#define PARTGUI_DLGTEXTTOWIRE_H

#include <QDialog>
#include <QPainterPath>
#include <vector>
#include <memory>
#include <QTextStream>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

namespace PartGui {

class Ui_DlgTextToWire;
class DlgTextToWire : public QWidget
{
    Q_OBJECT

public:
    explicit DlgTextToWire(QWidget* parent = nullptr);
    ~DlgTextToWire() override;
    bool apply();

private Q_SLOTS:
    void changeFont();

private:
    QFont makeFont() const;
    void createWires(const QString& text);
    QPainterPath createWires(const QFont& font, const QString& text);
    void createWires(const QPainterPath& path, QTextStream& out);
    QString makePoint(const Base::Vector3d& pnt) const;
    QString makePoints(const std::vector<Base::Vector3d>& pts) const;
    QString makeLineSegment(const std::vector<Base::Vector3d>& pts) const;
    QString makeCubicCurve(const std::vector<Base::Vector3d>& pts) const;
    QString makeWire() const;

private:
    std::unique_ptr<Ui_DlgTextToWire> ui;
    Q_DISABLE_COPY_MOVE(DlgTextToWire)
};

class TaskTextToWire : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskTextToWire();

public:
    bool accept() override;
    bool reject() override;
    void clicked(int id) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override;

private:
    DlgTextToWire* widget;
};

} // namespace PartGui

#endif // PARTGUI_DLGTEXTTOWIRE_H
