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

#include <QCoreApplication>
#include <QProgressDialog>
#include <QThread>
#include "ProgressIndicator.h"


using namespace PartGui;

/*!
  \code
  #include <XSControl_WorkSession.hxx>
  #include <Transfer_TransientProcess.hxx>

  STEPControl_Reader aReader;
  Handle(Message_ProgressIndicator) pi = new ProgressIndicator();

  aReader.ReadFile("myfile.stp");
  aReader.TransferRoots(pi->Start());

  \endcode
 */

ProgressIndicator::ProgressIndicator (const QString& title, QWidget* parent)
    : myProgress{new QProgressDialog(parent)}
{
    const int min = 0;
    const int max = 100;
    myProgress->setWindowTitle(title);
    myProgress->setAttribute(Qt::WA_DeleteOnClose);
    myProgress->setRange(min, max);
}

ProgressIndicator::~ProgressIndicator ()
{
    myProgress->close();
}

void ProgressIndicator::Show (const Message_ProgressScope& theScope,
                              const Standard_Boolean isForce)
{
    (void)isForce;
    Standard_CString aName = theScope.Name(); //current step
    myProgress->setLabelText (QString::fromUtf8(aName ? aName : "Processing..."));
    int current = static_cast<int>(100. * theScope.Value() / theScope.MaxValue());
    if (current != steps) {
        steps = current;
        myProgress->setValue(steps);
        QCoreApplication::processEvents();
    }

    myProgress->show();
}

Standard_Boolean ProgressIndicator::UserBreak()
{
    QThread *currentThread = QThread::currentThread();
    if (currentThread == myProgress->thread()) {
        if (canceled) {
            return true;
        }

        canceled = myProgress->wasCanceled();
        return canceled;
    }

    return false;
}

void ProgressIndicator::Reset()
{
    steps = 0;
    canceled = false;
}
