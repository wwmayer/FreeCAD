/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H
#define GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H

#include <Gui/PropertyPage.h>
#include <memory>

class QDoubleSpinBox;

namespace Gui
{
namespace Dialog
{
class Ui_DlgSettings3DView;

/**
 * The DlgSettings3DViewImp class implements a preference page to change settings
 * for the Inventor viewer.
 * \author Jürgen Riegel
 */
class DlgSettings3DViewImp: public PreferencePage
{
    Q_OBJECT

public:
    explicit DlgSettings3DViewImp(QWidget* parent = nullptr);
    ~DlgSettings3DViewImp() override;

    void saveSettings() override;
    void loadSettings() override;
    void resetSettingsToDefaults() override;

private Q_SLOTS:
    void onAliasingChanged(int);

protected:
    void changeEvent(QEvent* e) override;

private:
    void addAntiAliasing();
    void saveAntiAliasing();
    void loadAntiAliasing();
    void saveRenderCache();
    void loadRenderCache();
    void saveMarkerSize();
    void loadMarkerSize();

private:
    std::unique_ptr<Ui_DlgSettings3DView> ui;

    Q_DISABLE_COPY_MOVE(DlgSettings3DViewImp)
};

}  // namespace Dialog
}  // namespace Gui

#endif  // GUI_DIALOG_DLGSETTINGS3DVIEWIMP_H
