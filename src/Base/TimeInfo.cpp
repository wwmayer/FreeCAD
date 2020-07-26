/***************************************************************************
 *   Copyright (c) 2011 Jürgen Riegel <juergen.riegel@web.de>              *
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

#ifndef _PreComp_
# include <clocale>
# include <ctime>
# include <iomanip>
# include <sstream>
# include <QDateTime>
#endif

#include "TimeInfo.h"


using namespace Base;


/**
 * A constructor.
 * A more elaborate description of the constructor.
 */
TimeInfo::TimeInfo()
{
    setCurrent();
}

/**
 * A destructor.
 * A more elaborate description of the destructor.
 */
TimeInfo::~TimeInfo()
{
}


//**************************************************************************
// separator for other implementation aspects

void TimeInfo::setCurrent(void)
{
    tp = std::chrono::system_clock::now();
}

void TimeInfo::setTime_t (uint64_t sec)
{
    tp = std::chrono::time_point<std::chrono::system_clock>()
       + std::chrono::seconds(sec);
}

uint64_t TimeInfo::getSeconds(void) const
{
    int64_t sec = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
    return sec;
}

unsigned short TimeInfo::getMiliseconds(void) const
{
    int64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    unsigned short v = ms % 1000;
    return v;
}

std::string TimeInfo::toString(std::string fmt) const
{
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::stringstream str;
    str.imbue(std::locale("C"));
    if (fmt.empty()) {
        str << std::put_time(std::localtime(&tt), "%c");
    }
    else {
        str << std::put_time(std::localtime(&tt), fmt.c_str());
    }

    return str.str();
}

std::string TimeInfo::currentDateTimeString()
{
#if 0
    TimeInfo ti;
    std::time_t tt = std::chrono::system_clock::to_time_t(ti.tp);
    std::stringstream str;
    str.imbue(std::locale("C"));
    str << std::put_time(std::gmtime(&tt), "%FT%TZ");
    return str.str();
#elif (QT_VERSION >= 0x050300)
    return QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC)
        .toString(Qt::ISODate).toStdString();
#else
    QDateTime local = QDateTime::currentDateTime();
    QDateTime utc = local.toUTC();
    utc.setTimeSpec(Qt::LocalTime);
    int utcOffset = utc.secsTo(local);
    local.setUtcOffset(utcOffset);
    QString dm = local.toString(Qt::ISODate);
    return dm.toStdString();
#endif
}

std::string TimeInfo::diffTime(const TimeInfo &timeStart, const TimeInfo &timeEnd )
{
    std::stringstream str;
    str << diffTimeF(timeStart, timeEnd);
    return str.str();
}

double TimeInfo::diffTimeF(const TimeInfo &timeStart, const TimeInfo &timeEnd )
{
    auto diff = timeEnd.tp - timeStart.tp;
    return std::chrono::duration<double>(diff).count();
}

TimeInfo TimeInfo::null()
{
    TimeInfo ti;
    ti.tp = std::chrono::time_point<std::chrono::system_clock>();
    return ti;
}

bool TimeInfo::isNull() const
{
    return (*this) == TimeInfo::null();
}
