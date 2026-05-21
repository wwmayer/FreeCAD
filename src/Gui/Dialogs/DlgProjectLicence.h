// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2026 Werner Mayer <wmayer[at]users.sourceforge.net>
// SPDX-FileNotice: Part of the xwzCAD project.

/***************************************************************************
 *                                                                         *
 *   xwzCAD is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 2.1         *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 *   xwzCAD is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty           *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               *
 *   See the GNU Lesser General Public License for more details.           *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with xwzCAD. If not, see https://www.gnu.org/licenses   *
 *                                                                         *
 ***************************************************************************/

#ifndef GUI_DIALOG_DLGPROJECTLICENCE_H
#define GUI_DIALOG_DLGPROJECTLICENCE_H

#include <QDialog>
#include <memory>

class QComboBox;
class QGridLayout;
class QLabel;

namespace App {
class Document;
}

namespace Gui {

namespace Dialog {

class LicenseModel
{
public:
    explicit LicenseModel(App::Document* doc);
    void setLicencesToDocument(const QStringList& lics);
    QStringList getLicencesFromDocument() const;
    QByteArrayList getAvailableLicenses() const;

private:
    App::Document* doc;
};

class Ui_DlgProjectLicence;
class DlgProjectLicence : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(DlgProjectLicence)

public:
    explicit DlgProjectLicence(App::Document* doc,
                               QWidget* parent = nullptr,
                               Qt::WindowFlags fl = Qt::WindowFlags());
    ~DlgProjectLicence() override;
    void accept() override;

private:
    void setupDialog();
    void addLicence();
    void removeLicence();
    QStringList getLicencesFromDialog() const;
    void addComboBoxes(const QStringList& lics);
    void addStandardButtons(int numLicences);
    void addLicenceItems(QComboBox* cb);
    void setLicenceIndex(QComboBox* cb, const QString& lic);
    void setCurrentIndex(QComboBox* cb, const QStringList& used);

private:
    LicenseModel model;
    const int maxLicences = 10;
    QGridLayout* gridLayout;
    QLabel* textLabel;
    QPushButton* addButton;
    QList< QPair<QComboBox*, QPushButton*> > buttonMap;
    std::unique_ptr<Ui_DlgProjectLicence> ui;
};

} // namespace Dialog
} // namespace Gui


#endif // GUI_DIALOG_DLGPROJECTLICENCE_H

