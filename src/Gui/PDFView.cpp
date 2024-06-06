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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QAction>
#include <QEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>
#include <QStandardPaths>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>
#endif

#include <QPdfBookmarkModel>
#include <QPdfPageNavigation>
#include <QPdfSelection>
#include <QPdfSearchModel>
#include <QPdfDocument>
#include <QPdfView>

#include "PDFView.h"
#include "BitmapFactory.h"

using namespace Gui;

namespace
{
const qreal zoomMultiplier = std::sqrt(2.0);
}

PageSelector::PageSelector(QWidget *parent)
    : QWidget(parent)
    , pageNavigation(nullptr)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    previousPageButton = new QToolButton(this);
    previousPageButton->setText(QLatin1String("<"));
    previousPageButton->setEnabled(false);

    pageNumberEdit = new QLineEdit(this);
    pageNumberEdit->setAlignment(Qt::AlignRight);

    pageCountLabel = new QLabel(this);
    pageCountLabel->setText(QLatin1String("0"));

    nextPageButton = new QToolButton(this);
    nextPageButton->setText(QLatin1String(">"));
    nextPageButton->setEnabled(false);

    layout->addWidget(previousPageButton);
    layout->addWidget(pageNumberEdit);
    layout->addWidget(pageCountLabel);
    layout->addWidget(nextPageButton);
}

void PageSelector::setPageNavigation(QPdfPageNavigation *pageNav)
{
    // clang-format off
    pageNavigation = pageNav;

    connect(previousPageButton, &QToolButton::clicked,
            pageNavigation, &QPdfPageNavigation::goToPreviousPage);
    connect(pageNavigation, &QPdfPageNavigation::canGoToPreviousPageChanged,
            previousPageButton, &QToolButton::setEnabled);
    connect(pageNavigation, &QPdfPageNavigation::currentPageChanged,
            this, &PageSelector::onCurrentPageChanged);
    connect(pageNavigation, &QPdfPageNavigation::pageCountChanged,
            this, [this](int pageCount) {
        pageCountLabel->setText(QString::fromLatin1("/ %1").arg(pageCount));
    });

    connect(pageNumberEdit, &QLineEdit::editingFinished,
            this, &PageSelector::pageNumberEdited);
    connect(nextPageButton, &QToolButton::clicked,
            pageNavigation, &QPdfPageNavigation::goToNextPage);
    connect(pageNavigation, &QPdfPageNavigation::canGoToNextPageChanged,
            nextPageButton, &QToolButton::setEnabled);

    onCurrentPageChanged(pageNavigation->currentPage());
    // clang-format on
}

void PageSelector::onCurrentPageChanged(int page)
{
    if (pageNavigation->pageCount() == 0) {
        pageNumberEdit->setText(QString::number(0));
    }
    else {
        pageNumberEdit->setText(QString::number(page + 1));
    }
}

void PageSelector::pageNumberEdited()
{
    if (!pageNavigation) {
        return;
    }

    const QString text = pageNumberEdit->text();

    bool ok = false;
    const int pageNumber = text.toInt(&ok);

    if (!ok) {
        onCurrentPageChanged(pageNavigation->currentPage());
    }
    else {
        pageNavigation->setCurrentPage(qBound(0, pageNumber - 1, pageNavigation->pageCount() - 1));
    }
}

// ------------------------------------------------------------------------------------------------

ZoomSelector::ZoomSelector(QWidget *parent)
    : QComboBox(parent)
{
    // clang-format off
    setEditable(true);

    addItem(QLatin1String("Fit Width"));
    addItem(QLatin1String("Fit Page"));
    addItem(QLatin1String("50%"));
    addItem(QLatin1String("70%"));
    addItem(QLatin1String("85%"));
    addItem(QLatin1String("100%"));
    addItem(QLatin1String("125%"));
    addItem(QLatin1String("150%"));
    addItem(QLatin1String("175%"));
    addItem(QLatin1String("200%"));
    addItem(QLatin1String("400%"));

    connect(this, &QComboBox::currentTextChanged,
            this, &ZoomSelector::onCurrentTextChanged);

    connect(lineEdit(), &QLineEdit::editingFinished,
            this, [this](){onCurrentTextChanged(lineEdit()->text());
    });
    // clang-format on
}

void ZoomSelector::setZoomFactor(qreal zoomFactor)
{
    setCurrentText(QString::number(qRound(zoomFactor * 100)) + QLatin1String("%"));
}

void ZoomSelector::reset()
{
    setCurrentIndex(5); // 100%
}

void ZoomSelector::onCurrentTextChanged(const QString &text)
{
    if (text == QLatin1String("Fit Width")) {
        Q_EMIT zoomModeChanged(QPdfView::FitToWidth);
    }
    else if (text == QLatin1String("Fit Page")) {
        Q_EMIT zoomModeChanged(QPdfView::FitInView);
    }
    else {
        qreal factor = 1.0;

        QString withoutPercent(text);
        withoutPercent.remove(QLatin1Char('%'));

        bool ok = false;
        const int zoomLevel = withoutPercent.toInt(&ok);
        if (ok) {
            factor = zoomLevel / 100.0;
        }

        Q_EMIT zoomModeChanged(QPdfView::CustomZoom);
        Q_EMIT zoomFactorChanged(factor);
    }
}

// ------------------------------------------------------------------------------------------------

PDFView::PDFView(QWidget* parent)
    : MDIView(nullptr, parent)
    , document(new QPdfDocument(this))
    , pdfView(nullptr)
    , zoomSelector(new ZoomSelector(this))
    , pageSelector(new PageSelector(this))
{
    setupUi();

    zoomSelector->setMaximumWidth(150);
    mainToolBar->insertWidget(actionZoom, zoomSelector);

    pageSelector->setMaximumWidth(150);
    mainToolBar->addWidget(pageSelector);
    pageSelector->setPageNavigation(pdfView->pageNavigation());
    zoomSelector->reset();

    QPdfBookmarkModel *bookmarkModel = new QPdfBookmarkModel(this);
    bookmarkModel->setDocument(document);

    bookmarkView->setModel(bookmarkModel);
    connect(bookmarkView, &QTreeView::activated,
            this, &PDFView::bookmarkSelected);

    tabWidget->setTabEnabled(1, false);

    pdfView->setDocument(document);
    connect(pdfView, &QPdfView::zoomFactorChanged,
            zoomSelector, &ZoomSelector::setZoomFactor);
}

void PDFView::setupUi()
{
    actionZoom = new QAction(this);
    actionZoom->setIcon(Gui::BitmapFactory().iconFromTheme("zoom-in"));
    actionZoomOut = new QAction(this);
    actionZoomOut->setIcon(Gui::BitmapFactory().iconFromTheme("zoom-out"));
    actionContinuous = new QAction(this);
    actionContinuous->setCheckable(true);

    mainToolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, mainToolBar);
    mainToolBar->addAction(actionZoomOut);
    mainToolBar->addAction(actionZoom);
    mainToolBar->addAction(actionContinuous);

    QWidget* centralWidget = new QWidget(this);

    QVBoxLayout* verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* widget = new QWidget(centralWidget);

    QVBoxLayout* verticalLayout_2 = new QVBoxLayout(widget);
    verticalLayout_2->setSpacing(0);
    verticalLayout_2->setContentsMargins(11, 11, 11, 11);
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);

    QSplitter* splitter = new QSplitter(widget);
    splitter->setOrientation(Qt::Horizontal);

    tabWidget = new QTabWidget(splitter);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
    tabWidget->setSizePolicy(sizePolicy);
    tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setDocumentMode(false);

    bookmarkTab = new QWidget();

    QVBoxLayout* verticalLayout_3 = new QVBoxLayout(bookmarkTab);
    verticalLayout_3->setSpacing(0);
    verticalLayout_3->setContentsMargins(11, 11, 11, 11);
    verticalLayout_3->setContentsMargins(2, 2, 2, 2);

    bookmarkView = new QTreeView(bookmarkTab);
    sizePolicy.setHeightForWidth(bookmarkView->sizePolicy().hasHeightForWidth());
    bookmarkView->setSizePolicy(sizePolicy);
    bookmarkView->setHeaderHidden(true);

    verticalLayout_3->addWidget(bookmarkView);

    tabWidget->addTab(bookmarkTab, QString());

    pagesTab = new QWidget();
    tabWidget->addTab(pagesTab, QString());
    splitter->addWidget(tabWidget);

    pdfView = new QPdfView(splitter);

    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(10);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(pdfView->sizePolicy().hasHeightForWidth());
    pdfView->setSizePolicy(sizePolicy1);
    splitter->addWidget(pdfView);

    verticalLayout_2->addWidget(splitter);
    verticalLayout->addWidget(widget);

    setCentralWidget(centralWidget);

    retranslateUi();
    tabWidget->setCurrentIndex(0);

    setupConnections();
}

void PDFView::setupConnections()
{
    // clang-format off
    connect(zoomSelector, &ZoomSelector::zoomModeChanged,
            pdfView, &QPdfView::setZoomMode);
    connect(zoomSelector, &ZoomSelector::zoomFactorChanged,
            pdfView, &QPdfView::setZoomFactor);
    connect(actionZoom, &QAction::triggered,
            this, &PDFView::onActionZoom_In_triggered);
    connect(actionZoomOut, &QAction::triggered,
            this, &PDFView::onActionZoom_Out_triggered);
    connect(actionContinuous, &QAction::triggered,
            this, &PDFView::onActionContinuous_triggered);
    // clang-format on
}

void PDFView::changeEvent(QEvent* ev)
{
    if (ev->type() == QEvent::LanguageChange) {
        retranslateUi();
    }

    MDIView::changeEvent(ev);
}

void PDFView::retranslateUi()
{
    actionZoom->setText(tr("Zoom In"));
    actionZoomOut->setText(tr("Zoom Out"));
    actionContinuous->setText(tr("Continuous"));
    tabWidget->setTabText(tabWidget->indexOf(bookmarkTab), tr("Bookmarks"));
    tabWidget->setTabText(tabWidget->indexOf(pagesTab), tr("Pages"));
}

bool PDFView::loadFile(const QString& fileName)
{
    document->load(fileName);
    const auto documentTitle = document->metaData(QPdfDocument::Title).toString();
    setWindowTitle(!documentTitle.isEmpty() ? documentTitle : tr("PDF Viewer"));

    return true;
}

void PDFView::bookmarkSelected(const QModelIndex &index)
{
    if (index.isValid()) {
        const int page = index.data(QPdfBookmarkModel::PageNumberRole).toInt();
        pdfView->pageNavigation()->setCurrentPage(page);
    }
}

void PDFView::onActionZoom_In_triggered()
{
    pdfView->setZoomFactor(pdfView->zoomFactor() * zoomMultiplier);
}

void PDFView::onActionZoom_Out_triggered()
{
    pdfView->setZoomFactor(pdfView->zoomFactor() / zoomMultiplier);
}

void PDFView::onActionContinuous_triggered()
{
    // clang-format off
    pdfView->setPageMode(actionContinuous->isChecked() ? QPdfView::MultiPage
                                                       : QPdfView::SinglePage);
    // clang-format on
}

#include "moc_PDFView.cpp"
