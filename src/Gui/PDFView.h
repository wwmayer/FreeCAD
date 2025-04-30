// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef GUI_PDF_VIEW_H
#define GUI_PDF_VIEW_H

#include <Gui/MDIView.h>
#include <QComboBox>
#include <QPdfView>

class QLabel;
class QLineEdit;
class QPdfSelection;
class QPdfDocument;
class QPdfPageNavigation;
class QPdfSearchModel;
class QToolButton;
class QTreeView;

namespace Gui {

class PageSelector : public QWidget
{
    Q_OBJECT

public:
    explicit PageSelector(QWidget *parent = nullptr);

    void setPageNavigation(QPdfPageNavigation *pageNavigation);

private:
    void onCurrentPageChanged(int page);
    void pageNumberEdited();

private:
    QPdfPageNavigation *pageNavigation;

    QLineEdit *pageNumberEdit;
    QLabel *pageCountLabel;
    QToolButton *previousPageButton;
    QToolButton *nextPageButton;
};

class ZoomSelector : public QComboBox
{
    Q_OBJECT

public:
    explicit ZoomSelector(QWidget *parent = nullptr);

public:
    void setZoomFactor(qreal zoomFactor);
    void reset();

Q_SIGNALS:
    void zoomModeChanged(QPdfView::ZoomMode zoomMode);
    void zoomFactorChanged(qreal zoomFactor);

private:
    void onCurrentTextChanged(const QString &text);
};

class GuiExport PDFView : public MDIView
{
    Q_OBJECT

public:
    explicit PDFView(QWidget* parent);
    bool loadFile(const QString &);

    const char *getName() const override {
        return "PDFView";
    }

protected:
    void changeEvent(QEvent* ev) override;

private:
    void setupUi();
    void retranslateUi();
    void setupConnections();
    void bookmarkSelected(const QModelIndex &index);
    void onActionZoom_In_triggered();
    void onActionZoom_Out_triggered();
    void onActionContinuous_triggered();

private:
    QPdfDocument *document = nullptr;
    QPdfView *pdfView = nullptr;
    ZoomSelector *zoomSelector;
    PageSelector *pageSelector;
    QTabWidget* tabWidget = nullptr;
    QWidget* bookmarkTab = nullptr;
    QWidget* pagesTab = nullptr;
    QTreeView* bookmarkView = nullptr;
    QToolBar* mainToolBar = nullptr;
    QAction *actionZoom = nullptr;
    QAction *actionZoomOut = nullptr;
    QAction *actionContinuous = nullptr;
};

}

#endif //GUI_PDF_VIEW_H
