/***************************************************************************
 *   Copyright (c) 2016 Qingfeng Xia <qingfeng.xia    iesensor.com>        *
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


#ifndef GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H
#define GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H

#include <App/PropertyStandard.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Selection.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Fem/App/FemSolverObject.h>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintFluidBoundary.h"

#include <QKeyEvent>

class Ui_TaskFemConstraintFluidBoundary;

namespace App {
class Property;
}

namespace Gui {
class ViewProvider;
}

namespace FemGui {

class TaskFemConstraintFluidBoundary : public TaskFemConstraint
{
    Q_OBJECT

public:
    TaskFemConstraintFluidBoundary(ViewProviderFemConstraintFluidBoundary *ConstraintView,QWidget *parent = nullptr);
    virtual ~TaskFemConstraintFluidBoundary();

    const Fem::FemSolverObject* getFemSolver() const;

    std::string getBoundaryType() const;
    std::string getSubtype() const;
    double getBoundaryValue() const;

    std::string getTurbulenceModel() const;
    std::string getTurbulenceSpecification() const;
    double getTurbulentIntensityValue() const;
    double getTurbulentLengthValue() const;

    bool getHeatTransfering() const;
    std::string getThermalBoundaryType() const;
    double getTemperatureValue() const;
    double getHeatFluxValue() const;
    double getHTCoeffValue() const;

    const std::string getReferences() const;
    const std::string getDirectionName() const;
    const std::string getDirectionObject() const;
    bool getReverse() const;

private Q_SLOTS:
    void onBoundaryTypeChanged();
    void onSubtypeChanged();
    void onBoundaryValueChanged(double);
    void onTurbulenceSpecificationChanged();
    void onThermalBoundaryTypeChanged();
    void onReferenceDeleted();
    void onButtonDirection(const bool pressed = true);
    void onCheckReverse(bool); // consider removing this slot as the UI is hidden
    void addToSelection();
    void removeFromSelection();

protected:
    bool event(QEvent *e);
    virtual void changeEvent(QEvent *e);

private:
    void updateBoundaryTypeUI();
    void updateSubtypeUI();
    void updateThermalBoundaryUI();
    void updateTurbulenceUI();
    void updateUI();

private:
    Ui_TaskFemConstraintFluidBoundary* ui;
    int dimension;  // -1: unknown, 2 for 2D and 3 for 3D
    Fem::FemSolverObject* pcSolver;
    App::PropertyBool* pHeatTransfering;
    App::PropertyEnumeration* pTurbulenceModel;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintFluidBoundary : public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    TaskDlgFemConstraintFluidBoundary(ViewProviderFemConstraintFluidBoundary *ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    virtual void open();
    virtual bool accept();
    virtual bool reject();

};

} //namespace FemGui

#endif // GUI_TASKVIEW_TaskFemConstraintFluidBoundary_H
