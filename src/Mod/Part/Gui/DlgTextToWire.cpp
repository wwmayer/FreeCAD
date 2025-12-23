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

#include "PreCompiled.h"
#include <QMessageBox>
#include "DlgTextToWire.h"
#include "ui_DlgTextToWire.h"
#include <Gui/CommandT.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <App/Document.h>

// This implementation is based on the proof of concept:
// https://forum.freecad.org/viewtopic.php?t=99999

using namespace PartGui;

DlgTextToWire::DlgTextToWire(QWidget* parent)
    : QWidget{parent}
    , ui {new Ui_DlgTextToWire}
{
    ui->setupUi(this);

    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged,
            this, &DlgTextToWire::changeFont);
    connect(ui->spinBox, qOverload<int>(&QSpinBox::valueChanged),
            this, &DlgTextToWire::changeFont);
    connect(ui->checkBoxBold, &QCheckBox::toggled,
            this, &DlgTextToWire::changeFont);
    connect(ui->checkBoxItalic, &QCheckBox::toggled,
            this, &DlgTextToWire::changeFont);
}

DlgTextToWire::~DlgTextToWire() = default;

void DlgTextToWire::changeFont()
{
    ui->plainTextEdit->setFont(makeFont());
}

QFont DlgTextToWire::makeFont() const
{
    QFont font = ui->fontComboBox->currentFont();
    font.setPointSize(ui->spinBox->value());
    font.setBold(ui->checkBoxBold->isChecked());
    font.setItalic(ui->checkBoxItalic->isChecked());
    return font;
}

bool DlgTextToWire::apply()
{
    const QString text = ui->plainTextEdit->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::warning(this, tr("No text"),
                             tr("Please add some text to the edit field first."));
        return false;
    }

    App::Document* activeDoc = App::GetApplication().getActiveDocument();
    activeDoc->openTransaction("Create shape string");

    try {
        createWires(text);
        activeDoc->commitTransaction();
        return true;
    }
    catch (const Base::Exception& e) {
        e.ReportException();
        activeDoc->abortTransaction();
        return false;
    }
}

void DlgTextToWire::createWires(const QString& text)
{
    QFont font = makeFont();
    QPainterPath path = createWires(font, text);
    if (ui->checkBoxSimple->isChecked()) {
        path = path.simplified();
    }

    QString str;
    QTextStream out(&str);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    out.setCodec("UTF-8");
#endif
    createWires(path, out);

    Gui::doCommandT(Gui::Command::App, str.toStdString());
}

QPainterPath DlgTextToWire::createWires(const QFont& font, const QString& text)
{
    QPainterPath path;
    QStringList lines = text.split(QString::fromLatin1("\n"));
    double ypos = 0.0;
    double spacing = 0.0;
    bool firstLine = true;
    for (const QString& line : std::as_const(lines)) {
        path.addText(0, ypos, font, line);
        QRectF box = path.boundingRect();
        if (firstLine) {
            firstLine = false;
            const double offset = 0.05;
            spacing = offset * box.height();
        }
        ypos = spacing + box.height();  // NOLINT
        if (line.isEmpty()) {
            // for an empty string the path's bounding box doesn't expand
            ypos += font.pointSizeF();
        }
    }
    return path;
}

void DlgTextToWire::createWires(const QPainterPath& path, QTextStream& out)
{
    if (path.elementCount() == 0) {
        return;
    }

    QTransform transform;
    transform.scale(1.0, -1.0);
    QPainterPath path_t = transform.map(path);

    auto makePoint = [](const QPainterPath::Element& item){
        return Base::Vector3d(item.x, item.y);
    };

    auto keepLast = [](std::vector<Base::Vector3d>& pts){
        Base::Vector3d pnt = pts.back();
        pts.clear();
        pts.emplace_back(pnt);
    };

    out << "from FreeCAD import Base\n";
    out << "edges = []\n";
    out << "wires = []\n";
    std::vector<Base::Vector3d> pts;
    for (int index = 0; index < path_t.elementCount(); index++) {
        if (path_t.elementAt(index).isMoveTo()) {
            if (!pts.empty()) {
                out << makeWire();
            }
            pts.clear();
            pts.emplace_back(makePoint(path_t.elementAt(index)));
        }
        else if (path_t.elementAt(index).isLineTo()) {
            pts.emplace_back(makePoint(path_t.elementAt(index)));
            out << makeLineSegment(pts);
            keepLast(pts);
        }
        else if (path_t.elementAt(index).isCurveTo()) {
            pts.emplace_back(makePoint(path_t.elementAt(index)));
            pts.emplace_back(makePoint(path_t.elementAt(index + 1)));
            pts.emplace_back(makePoint(path_t.elementAt(index + 2)));
            out << makeCubicCurve(pts);
            keepLast(pts);
        }
    }

    if (!pts.empty()) {
        out << makeWire();
    }

    out << "Part.show(Part.makeCompound(wires), 'Compound')\n";
}

QString DlgTextToWire::makePoint(const Base::Vector3d& pnt) const
{
    return QString::fromLatin1("Base.Vector(%1, %2)").arg(pnt.x).arg(pnt.y);
}

QString DlgTextToWire::makePoints(const std::vector<Base::Vector3d>& pts) const
{
    QStringList list;
    std::transform(pts.begin(),
                   pts.end(),
                   std::back_inserter(list),
                   [this](const Base::Vector3d& pnt) {
                       return makePoint(pnt);
                   });

    return QString::fromLatin1("[%1]").arg(list.join(QLatin1String(", ")));
}

QString DlgTextToWire::makeLineSegment(const std::vector<Base::Vector3d>& pts) const
{
    assert(pts.size() == 2);
    return QString::fromLatin1("edges.append(Part.makeLine(%1, %2))\n")
        .arg(makePoint(pts[0]), makePoint(pts[1]));
}

QString DlgTextToWire::makeCubicCurve(const std::vector<Base::Vector3d>& pts) const
{
    assert(pts.size() == 4);
    return QString::fromLatin1("geomCurve = Part.BezierCurve()\n"
                               "geomCurve.setPoles(%1)\n"
                               "edges.append(Part.Edge(geomCurve))\n")
        .arg(makePoints(pts));
}

QString DlgTextToWire::makeWire() const
{
    return QString::fromLatin1("wires.append(Part.Wire(edges))\n"
                               "edges = []\n");
}

// ----------------------------------------------------------------------------

TaskTextToWire::TaskTextToWire()
    : widget{new DlgTextToWire()}
{
    addTaskBox(QPixmap(), widget);
}

bool TaskTextToWire::accept()
{
    return widget->apply();
}

bool TaskTextToWire::reject()
{
    widget->close();
    return true;
}

void TaskTextToWire::clicked(int id)
{
    if (id == QDialogButtonBox::Apply) {
        widget->apply();
    }
}

QDialogButtonBox::StandardButtons TaskTextToWire::getStandardButtons() const
{
    // clang-format off
    return QDialogButtonBox::Ok |
           QDialogButtonBox::Apply |
           QDialogButtonBox::Close;
    // clang-format on
}

#include "moc_DlgTextToWire.cpp"
