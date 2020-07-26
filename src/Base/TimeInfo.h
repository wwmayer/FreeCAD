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


#ifndef BASE_TIMEINFO_H
#define BASE_TIMEINFO_H

#include <chrono>

namespace Base
{
/// BaseClass class and root of the type system
class BaseExport TimeInfo
{

public:
    /// Construction
    TimeInfo();
    /// Destruction
    ~TimeInfo();

    /// sets the object to the current system time
    void setCurrent(void);
    void setTime_t (uint64_t seconds);

    uint64_t getSeconds(void) const; 
    unsigned short  getMiliseconds(void) const; 

    //void operator =  (const TimeInfo &time); 
    bool operator == (const TimeInfo &time) const;
    bool operator != (const TimeInfo &time) const;

    bool operator <  (const TimeInfo &time) const;
    bool operator <= (const TimeInfo &time) const;
    bool operator >= (const TimeInfo &time) const;
    bool operator >  (const TimeInfo &time) const;

    std::string toString(std::string fmt = std::string()) const;
    static std::string currentDateTimeString();
    static std::string diffTime(const TimeInfo &timeStart,const TimeInfo &timeEnd = TimeInfo());
    static double diffTimeF(const TimeInfo &timeStart,const TimeInfo &timeEnd  = TimeInfo());
    bool isNull() const;
    static TimeInfo null();

private:
    std::chrono::time_point<std::chrono::system_clock> tp;
};

inline bool
TimeInfo::operator != (const TimeInfo &time) const
{
    return this->tp != time.tp;
}

inline bool
TimeInfo::operator == (const TimeInfo &time) const
{
    return this->tp == time.tp;
}

inline bool
TimeInfo::operator <  (const TimeInfo &time) const
{
    return this->tp < time.tp;
}

inline bool
TimeInfo::operator <= (const TimeInfo &time) const
{
    return this->tp <= time.tp;
}

inline bool
TimeInfo::operator >= (const TimeInfo &time) const
{
    return this->tp >= time.tp;
}

inline bool
TimeInfo::operator >  (const TimeInfo &time) const
{
    return this->tp > time.tp;
}

} //namespace Base


#endif // BASE_TIMEINFO_H

