// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 Andrew Shkolik <shkolik@gmail.com>                  *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QDoubleSpinBox>

#include <GeomAbs_Shape.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#endif

#include <App/Document.h>
#include <App/PropertyLinks.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/Selection/SelectionObject.h>
#include <Gui/Widgets.h>
#include <Mod/Part/Gui/ViewProvider.h>

#include "Gordon/TaskGordonSurface.h"

#include "Gordon/ui_TaskGordonSurface.h"
#include "Gordon/ViewProviderGordonSurface.h"


using namespace SurfaceGui;

namespace SurfaceGui
{

class GordonSurfacePanel::ShapeSelection: public Gui::SelectionFilterGate
{
public:
    FC_DISABLE_COPY_MOVE(ShapeSelection)
    ShapeSelection(GordonSurfacePanel::SelectionMode& mode,
                   GordonSurfacePanel::SelectionType& selectionType,
                   Surface::GordonSurface* editedObject)
        : Gui::SelectionFilterGate(nullPointer())
        , mode(mode)
        , selectionType(selectionType)
        , editedObject(editedObject)
    {}
    ~ShapeSelection() override
    {
        mode = GordonSurfacePanel::None;
    }
    /**
     * Allow the user to pick only edges.
     */
    bool allow(App::Document* doc, App::DocumentObject* pObj, const char* sSubName) override
    {
        Q_UNUSED(doc)

        // don't allow references to itself
        if (pObj == editedObject) {
            return false;
        }
        if (!pObj->isDerivedFrom<Part::Feature>()) {
            return false;
        }

        if (Base::Tools::isNullOrEmpty(sSubName)) {
            return false;
        }

        switch (mode) {
            case GordonSurfacePanel::AppendEdge:
                return selectionType == Guide
                    ? allowEdge(true, editedObject->GuideEdges, pObj, sSubName)
                    : allowEdge(true, editedObject->ProfileEdges, pObj, sSubName);
            case GordonSurfacePanel::RemoveEdge:
                return selectionType == Guide
                    ? allowEdge(false, editedObject->GuideEdges, pObj, sSubName)
                    : allowEdge(false, editedObject->ProfileEdges, pObj, sSubName);
            default:
                return false;
        }
    }

private:
    bool allowEdge(bool appendEdges,
                   const App::PropertyLinkSubList& edges,
                   App::DocumentObject* pObj,
                   const char* sSubName)
    {
        std::string element(sSubName);
        if (element.substr(0, 4) != "Edge") {
            return false;
        }

        auto links = edges.getSubListValues();

        for (const auto& it : links) {
            if (it.first == pObj) {
                for (const auto& jt : it.second) {
                    if (jt == sSubName) {
                        return !appendEdges;
                    }
                }
            }
        }

        return appendEdges;
    }

private:
    GordonSurfacePanel::SelectionMode& mode;
    GordonSurfacePanel::SelectionType& selectionType;
    Surface::GordonSurface* editedObject;
};

// ----------------------------------------------------------------------------

GordonSurfacePanel::GordonSurfacePanel(ViewProviderGordonSurface* vp, Surface::GordonSurface* obj)
    : selectionMode {None}
    , selectionType {Profile}
    , editedObject {obj}
    , checkCommand {true}
    , ui {new Ui_TaskGordonSurface()}
    , vp {vp}
{
    ui->setupUi(this);
    setupConnections();
    // ui->statusLabel->clear();

    setEditedObject(obj);

    // Create context menu
    QAction* action = new QAction(tr("Remove"), this);
    action->setShortcut(QStringLiteral("Del"));
    action->setShortcutContext(Qt::WidgetShortcut);
    ui->listProfiles->addAction(action);
    connect(action, &QAction::triggered, this, &GordonSurfacePanel::onDeleteProfile);
    ui->listProfiles->setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction* action1 = new QAction(tr("Remove"), this);
    action1->setShortcut(QStringLiteral("Del"));
    action1->setShortcutContext(Qt::WidgetShortcut);
    ui->listGuides->addAction(action1);
    connect(action1, &QAction::triggered, this, &GordonSurfacePanel::onDeleteGuide);
    ui->listGuides->setContextMenuPolicy(Qt::ActionsContextMenu);

    // clang-format off
    connect(ui->listProfiles->model(), &QAbstractItemModel::rowsMoved,
            this, &GordonSurfacePanel::onProfileIndexesMoved);
    connect(ui->listGuides->model(), &QAbstractItemModel::rowsMoved,
            this, &GordonSurfacePanel::onGuideIndexesMoved);
    // clang-format on
}

/*
 *  Destroys the object and frees any allocated resources
 */
GordonSurfacePanel::~GordonSurfacePanel()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
}

void GordonSurfacePanel::setupConnections()
{
    // clang-format off
    connect(ui->buttonProfileAdd, &QToolButton::toggled,
            this, &GordonSurfacePanel::onButtonProfileAddToggled);
    connect(ui->buttonProfileRemove, &QToolButton::toggled,
            this, &GordonSurfacePanel::onButtonProfileRemoveToggled);

    connect(ui->buttonGuideAdd, &QToolButton::toggled,
            this, &GordonSurfacePanel::onButtonGuideAddToggled);
    connect(ui->buttonGuideRemove, &QToolButton::toggled,
            this, &GordonSurfacePanel::onButtonGuideRemoveToggled);
    connect(ui->toleranceSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &GordonSurfacePanel::onToleranceChanged);
    // clang-format on
}

void GordonSurfacePanel::appendButtons(Gui::ButtonGroup* buttonGroup)
{
    buttonGroup->addButton(ui->buttonProfileAdd, int(SelectionMode::AppendEdge));
    buttonGroup->addButton(ui->buttonProfileRemove, int(SelectionMode::RemoveEdge));
    buttonGroup->addButton(ui->buttonGuideAdd, int(SelectionMode::AppendEdge));
    buttonGroup->addButton(ui->buttonGuideRemove, int(SelectionMode::RemoveEdge));
}

void GordonSurfacePanel::setEditedObject(Surface::GordonSurface* fea)
{
    editedObject = fea;
    App::Document* doc = editedObject->getDocument();


    // get the profiles
    auto profilesObjects = editedObject->ProfileEdges.getValues();
    auto profileEdges = editedObject->ProfileEdges.getSubValues();

    for (std::size_t i = 0; i < profilesObjects.size(); i++) {
        App::DocumentObject* obj = profilesObjects[i];
        const std::string& edge = profileEdges[i];

        QListWidgetItem* item = new QListWidgetItem(ui->listProfiles);
        ui->listProfiles->addItem(item);

        QString text = QStringLiteral("%1.%2").arg(QString::fromUtf8(obj->Label.getValue()),
                                                   QString::fromStdString(edge));
        item->setText(text);

        // The user data field of a list widget item
        // is a list of 3 elements:
        // 1. document name
        // 2. object name
        // 3. sub-element name of the edge
        QList<QVariant> data;
        data << QByteArray(doc->getName());
        data << QByteArray(obj->getNameInDocument());
        data << QByteArray(edge.c_str());
        item->setData(Qt::UserRole, data);
    }

    // get the guides
    auto guidesObjects = editedObject->GuideEdges.getValues();
    auto guidesEdges = editedObject->GuideEdges.getSubValues();

    for (std::size_t i = 0; i < guidesObjects.size(); i++) {
        App::DocumentObject* obj = guidesObjects[i];
        const std::string& edge = guidesEdges[i];

        QListWidgetItem* item = new QListWidgetItem(ui->listGuides);
        ui->listGuides->addItem(item);

        QString text = QStringLiteral("%1.%2").arg(QString::fromUtf8(obj->Label.getValue()),
                                                   QString::fromStdString(edge));
        item->setText(text);

        // The user data field of a list widget item
        // is a list of 3 elements:
        // 1. document name
        // 2. object name
        // 3. sub-element name of the edge
        QList<QVariant> data;
        data << QByteArray(doc->getName());
        data << QByteArray(obj->getNameInDocument());
        data << QByteArray(edge.c_str());
        item->setData(Qt::UserRole, data);
    }

    double tol = editedObject->Tolerance.getValue();
    ui->toleranceSpinBox->setValue(tol < Precision::Confusion() ? Precision::Confusion() : tol);

    // attach this document observer
    attachDocument(Gui::Application::Instance->getDocument(doc));
}

void GordonSurfacePanel::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QWidget::changeEvent(e);
    }
}

void GordonSurfacePanel::open()
{
    checkOpenCommand();

    // highlight all edges
    this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), true);
    this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), true);

    clearSelection();

    // if the surface is not yet created then automatically start "AppendEdge" mode
    if (editedObject->Shape.getShape().isNull()) {
        ui->buttonProfileAdd->setChecked(true);
    }
}

void GordonSurfacePanel::clearSelection()
{
    Gui::Selection().clearSelection();
}

void GordonSurfacePanel::checkOpenCommand()
{
    if (checkCommand && !Gui::Command::hasPendingCommand()) {
        std::string Msg("Edit ");
        Msg += editedObject->Label.getValue();
        Gui::Command::openCommand(Msg.c_str());
        checkCommand = false;
    }
}

void GordonSurfacePanel::slotUndoDocument(const Gui::Document& doc)
{
    Q_UNUSED(doc)
    checkCommand = true;
}

void GordonSurfacePanel::slotRedoDocument(const Gui::Document& doc)
{
    Q_UNUSED(doc)
    checkCommand = true;
}

void GordonSurfacePanel::slotDeletedObject(const Gui::ViewProviderDocumentObject& Obj)
{
    // If this view provider is being deleted then reset the colors of
    // referenced part objects. The dialog will be deleted later.
    if (this->vp == &Obj) {
        this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), false);
        this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), false);
    }
}

bool GordonSurfacePanel::accept()
{
    selectionMode = None;
    Gui::Selection().rmvSelectionGate();

    if (editedObject->mustExecute()) {
        editedObject->recomputeFeature();
    }
    if (!editedObject->isValid()) {
        QMessageBox::warning(this,
                             tr("Invalid object"),
                             QString::fromLatin1(editedObject->getStatusString()));
        return false;
    }

    this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), false);
    this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), false);

    return true;
}

bool GordonSurfacePanel::reject()
{
    if (!editedObject.expired()) {
        this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), false);
        this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), false);
    }

    selectionMode = None;
    Gui::Selection().rmvSelectionGate();

    return true;
}

void GordonSurfacePanel::onButtonProfileAddToggled(bool checked)
{
    if (checked) {
        selectionType = Profile;
        // 'selectionMode' is passed by reference and changed when the filter is deleted
        Gui::Selection().addSelectionGate(
            new ShapeSelection(selectionMode, selectionType, editedObject.get()));
        selectionMode = AppendEdge;
    }
    else if (selectionType == Profile && selectionMode == AppendEdge) {
        exitSelectionMode();
    }
}

void GordonSurfacePanel::onButtonProfileRemoveToggled(bool checked)
{
    if (checked) {
        selectionType = Profile;
        // 'selectionMode' is passed by reference and changed when the filter is deleted
        Gui::Selection().addSelectionGate(
            new ShapeSelection(selectionMode, selectionType, editedObject.get()));
        selectionMode = RemoveEdge;
    }
    else if (selectionType == Profile && selectionMode == RemoveEdge) {
        exitSelectionMode();
    }
}

void GordonSurfacePanel::onButtonGuideAddToggled(bool checked)
{
    if (checked) {
        selectionType = Guide;
        // 'selectionMode' is passed by reference and changed when the filter is deleted
        Gui::Selection().addSelectionGate(
            new ShapeSelection(selectionMode, selectionType, editedObject.get()));
        selectionMode = AppendEdge;
    }
    else if (selectionType == Guide && selectionMode == AppendEdge) {
        exitSelectionMode();
    }
}

void GordonSurfacePanel::onButtonGuideRemoveToggled(bool checked)
{
    if (checked) {
        selectionType = Guide;
        // 'selectionMode' is passed by reference and changed when the filter is deleted
        Gui::Selection().addSelectionGate(
            new ShapeSelection(selectionMode, selectionType, editedObject.get()));
        selectionMode = RemoveEdge;
    }
    else if (selectionType == Guide && selectionMode == RemoveEdge) {
        exitSelectionMode();
    }
}

void GordonSurfacePanel::appendEdges(const Gui::SelectionChanges& msg,
                                     QListWidget* list,
                                     App::PropertyLinkSubList& edges)
{
    QListWidgetItem* item = new QListWidgetItem(list);
    list->addItem(item);

    Gui::SelectionObject sel(msg);
    QString text = QStringLiteral("%1.%2").arg(QString::fromUtf8(sel.getObject()->Label.getValue()),
                                               QString::fromLatin1(msg.pSubName));
    item->setText(text);

    QList<QVariant> data;
    data << QByteArray(msg.pDocName);
    data << QByteArray(msg.pObjectName);
    data << QByteArray(msg.pSubName);
    item->setData(Qt::UserRole, data);

    auto objects = edges.getValues();
    objects.push_back(sel.getObject());
    auto element = edges.getSubValues();
    element.emplace_back(msg.pSubName);
    edges.setValues(objects, element);

    this->vp->highlightReferences(edges.getSubListValues(), true);
}

void GordonSurfacePanel::removeEdge(const Gui::SelectionChanges& msg,
                                    QListWidget* list,
                                    App::PropertyLinkSubList& edges)
{
    Gui::SelectionObject sel(msg);
    QList<QVariant> data;
    data << QByteArray(msg.pDocName);
    data << QByteArray(msg.pObjectName);
    data << QByteArray(msg.pSubName);

    // only the three first elements must match
    for (int i = 0; i < list->count(); i++) {
        QListWidgetItem* item = list->item(i);
        QList<QVariant> userdata = item->data(Qt::UserRole).toList();
        if (userdata == data) {
            list->takeItem(i);
            delete item;
            break;
        }
    }

    this->vp->highlightReferences(edges.getSubListValues(), false);
    App::DocumentObject* obj = sel.getObject();
    std::string sub = msg.pSubName;
    auto objects = edges.getValues();
    auto element = edges.getSubValues();
    auto it = objects.begin();
    auto jt = element.begin();

    for (; it != objects.end() && jt != element.end(); ++it, ++jt) {
        if (*it == obj && *jt == sub) {
            objects.erase(it);
            element.erase(jt);
            edges.setValues(objects, element);
            break;
        }
    }
    this->vp->highlightReferences(edges.getSubListValues(), true);
}


void GordonSurfacePanel::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (selectionMode == None) {
        return;
    }

    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        checkOpenCommand();
        if (selectionMode == AppendEdge) {
            if (selectionType == Profile) {
                appendEdges(msg, ui->listProfiles, editedObject->ProfileEdges);
            }
            else {
                appendEdges(msg, ui->listGuides, editedObject->GuideEdges);
            }
        }
        else if (selectionMode == RemoveEdge) {
            if (selectionType == Profile) {
                removeEdge(msg, ui->listProfiles, editedObject->ProfileEdges);
            }
            else {
                removeEdge(msg, ui->listGuides, editedObject->GuideEdges);
            }
        }

        // editedObject->recomputeFeature();
        QTimer::singleShot(50, this, &GordonSurfacePanel::clearSelection);
    }
}

void GordonSurfacePanel::onDeleteProfile()
{
    int row = ui->listProfiles->currentRow();
    QListWidgetItem* item = ui->listProfiles->item(row);
    if (item) {
        checkOpenCommand();
        QList<QVariant> data;
        data = item->data(Qt::UserRole).toList();
        ui->listProfiles->takeItem(row);
        delete item;

        App::Document* doc = App::GetApplication().getDocument(data[0].toByteArray());
        App::DocumentObject* obj = doc ? doc->getObject(data[1].toByteArray()) : nullptr;
        std::string sub = data[2].toByteArray().constData();
        auto objects = editedObject->ProfileEdges.getValues();
        auto element = editedObject->ProfileEdges.getSubValues();
        auto it = objects.begin();
        auto jt = element.begin();
        this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), false);
        for (; it != objects.end() && jt != element.end(); ++it, ++jt) {
            if (*it == obj && *jt == sub) {
                objects.erase(it);
                element.erase(jt);
                editedObject->ProfileEdges.setValues(objects, element);
                break;
            }
        }
        this->vp->highlightReferences(editedObject->ProfileEdges.getSubListValues(), true);

        // editedObject->recomputeFeature();
    }
}

void GordonSurfacePanel::onDeleteGuide()
{
    int row = ui->listGuides->currentRow();
    QListWidgetItem* item = ui->listGuides->item(row);
    if (item) {
        checkOpenCommand();
        QList<QVariant> data;
        data = item->data(Qt::UserRole).toList();
        ui->listGuides->takeItem(row);
        delete item;

        App::Document* doc = App::GetApplication().getDocument(data[0].toByteArray());
        App::DocumentObject* obj = doc ? doc->getObject(data[1].toByteArray()) : nullptr;
        std::string sub = data[2].toByteArray().constData();
        auto objects = editedObject->GuideEdges.getValues();
        auto element = editedObject->GuideEdges.getSubValues();
        auto it = objects.begin();
        auto jt = element.begin();
        this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), false);
        for (; it != objects.end() && jt != element.end(); ++it, ++jt) {
            if (*it == obj && *jt == sub) {
                objects.erase(it);
                element.erase(jt);
                editedObject->GuideEdges.setValues(objects, element);
                break;
            }
        }
        this->vp->highlightReferences(editedObject->GuideEdges.getSubListValues(), true);

        // editedObject->recomputeFeature();
    }
}

void GordonSurfacePanel::onProfileIndexesMoved()
{
    QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>(sender());
    if (!model) {
        return;
    }

    std::vector<App::DocumentObject*> objects;
    std::vector<std::string> element;

    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        QModelIndex index = model->index(i, 0);
        QList<QVariant> data;
        data = index.data(Qt::UserRole).toList();

        App::Document* doc = App::GetApplication().getDocument(data[0].toByteArray());
        App::DocumentObject* obj = doc ? doc->getObject(data[1].toByteArray()) : nullptr;
        std::string sub = data[2].toByteArray().constData();

        objects.push_back(obj);
        element.push_back(sub);
    }

    editedObject->ProfileEdges.setValues(objects, element);
    // editedObject->recomputeFeature();
}

void GordonSurfacePanel::onGuideIndexesMoved()
{
    QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>(sender());
    if (!model) {
        return;
    }

    std::vector<App::DocumentObject*> objects;
    std::vector<std::string> element;

    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        QModelIndex index = model->index(i, 0);
        QList<QVariant> data;
        data = index.data(Qt::UserRole).toList();

        App::Document* doc = App::GetApplication().getDocument(data[0].toByteArray());
        App::DocumentObject* obj = doc ? doc->getObject(data[1].toByteArray()) : nullptr;
        std::string sub = data[2].toByteArray().constData();

        objects.push_back(obj);
        element.push_back(sub);
    }

    editedObject->GuideEdges.setValues(objects, element);
    // editedObject->recomputeFeature();
}

void GordonSurfacePanel::onToleranceChanged(double tolerance)
{
    editedObject->Tolerance.setValue(tolerance);
}

void GordonSurfacePanel::exitSelectionMode()
{
    // 'selectionMode' is passed by reference to the filter and changed when the filter is deleted
    Gui::Selection().clearSelection();
    Gui::Selection().rmvSelectionGate();
}

// ----------------------------------------------------------------------------

TaskGordonSurface::TaskGordonSurface(ViewProviderGordonSurface* vp, Surface::GordonSurface* obj)
{
    // Set up button group
    buttonGroup = new Gui::ButtonGroup(this);
    buttonGroup->setExclusive(true);

    // first task box
    widget = new GordonSurfacePanel(vp, obj);
    widget->appendButtons(buttonGroup);
    addTaskBox(Gui::BitmapFactory().pixmap("Surface_GordonSurface"), widget);
}

void TaskGordonSurface::setEditedObject(Surface::GordonSurface* obj)
{
    widget->setEditedObject(obj);
}

void TaskGordonSurface::open()
{
    widget->open();
}

void TaskGordonSurface::closed()
{
    widget->reject();
}

bool TaskGordonSurface::accept()
{
    bool ok = widget->accept();
    if (ok) {
        Gui::Command::commitCommand();
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
        Gui::Command::updateActive();
    }

    return ok;
}

bool TaskGordonSurface::reject()
{
    bool ok = widget->reject();
    if (ok) {
        Gui::Command::abortCommand();
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
        Gui::Command::updateActive();
    }

    return ok;
}

}  // namespace SurfaceGui
