// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2026 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "FileTemplate.h"


using namespace Base;


//**************************************************************************
// Construction/Destruction

/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
ClassTemplate::ClassTemplate() = default;

ClassTemplate::ClassTemplate(const ClassTemplate&) = default;

ClassTemplate::ClassTemplate(ClassTemplate&&) noexcept = default;

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
ClassTemplate::~ClassTemplate() = default;

ClassTemplate& ClassTemplate::operator=(const ClassTemplate&) = default;

ClassTemplate& ClassTemplate::operator=(ClassTemplate&&) noexcept = default;

//**************************************************************************
// separator for other implementation aspects

/**
 * a normal member taking two arguments and returning an integer value.
 * \par
 * You can use a printf like interface like:
 * \code
 * GetConsole().Warning("Some defects in %s, loading anyway\n",str);
 * \endcode
 * @param a an integer argument.
 * @param s a constant character pointer.
 * @see ClassTemplate()
 * @see ~ClassTemplate()
 * @see testMeToo()
 * @see publicVar()
 * @return The test results
 */
int ClassTemplate::testMe(int /*a*/, const char* /*s*/)
{
    return 0;
}


//**************************************************************************
//**************************************************************************
// Separator for additional classes
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
