/***************************************************************************
 *   Copyright (c) 2022 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "PreCompiled.h"

#include "ParameterObserver.h"

using namespace Base;

ParameterObserver::ParameterObserver()
{
}

void ParameterObserver::initParameters()
{
    for (const auto&it : parameters) {
        handle->Notify(it.first);
    }
}

void ParameterObserver::OnChange(Base::Subject<const char*> &, const char* sReason)
{
    if (!sReason)
        return;
    auto it = parameters.find(sReason);
    if (it == parameters.end())
        return;
    it->second.fetch(handle, sReason);
}

void ParameterObserver::addParameter(const char* key, const Object& value)
{
    parameters.emplace(std::make_pair(key, value));
}

void ParameterObserver::setBoolean(const char* key, bool value)
{
    parameters.at(key).setParameter(handle, key, value);
    parameters.at(key).setValue(value);
}

bool ParameterObserver::getBoolean(const char* key) const
{
    return std::get<bool>(parameters.at(key).value());
}

bool ParameterObserver::getDefaultBoolean(const char* key) const
{
    return std::get<bool>(parameters.at(key).defaultValue());
}

void ParameterObserver::setInt(const char* key, long value)
{
    parameters.at(key).setParameter(handle, key, value);
    parameters.at(key).setValue(value);
}

long ParameterObserver::getInt(const char* key) const
{
    return std::get<long>(parameters.at(key).value());
}

long ParameterObserver::getDefaultInt(const char* key) const
{
    return std::get<long>(parameters.at(key).defaultValue());
}

void ParameterObserver::setUnsigned(const char* key, unsigned long value)
{
    parameters.at(key).setParameter(handle, key, value);
    parameters.at(key).setValue(value);
}

unsigned long ParameterObserver::getUnsigned(const char* key) const
{
    return std::get<unsigned long>(parameters.at(key).value());
}

unsigned long ParameterObserver::getDefaultUnsigned(const char* key) const
{
    return std::get<unsigned long>(parameters.at(key).defaultValue());
}

void ParameterObserver::setFloat(const char* key, double value)
{
    parameters.at(key).setParameter(handle, key, value);
    parameters.at(key).setValue(value);
}

double ParameterObserver::getFloat(const char* key) const
{
    return std::get<double>(parameters.at(key).value());
}

double ParameterObserver::getDefaultFloat(const char* key) const
{
    return std::get<double>(parameters.at(key).defaultValue());
}

void ParameterObserver::setString(const char* key, const char* value)
{
    parameters.at(key).setParameter(handle, key, value);
    parameters.at(key).setValue(value);
}

std::string ParameterObserver::getString(const char* key) const
{
    return std::get<std::string>(parameters.at(key).value());
}

std::string ParameterObserver::getDefaultString(const char* key) const
{
    return std::get<std::string>(parameters.at(key).defaultValue());
}
