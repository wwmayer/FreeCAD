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

// clang-format off
#ifndef BASE_SAFE_H
#define BASE_SAFE_H

#include <Base/Console.h>
#include <Base/Exception.h>
#include <stdexcept>
#include <QObject>
#include <FCGlobal.h>


struct ParseArgs
{
    // NOLINTBEGIN
    static void get(std::vector<void*>& v) {
        (void)v;
    }

    template<typename T1>
    static void get(std::vector<void*>& v, const T1& t1) {
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t1))));
    }

    template<typename T1, typename T2>
    static void get(std::vector<void*>& v, const T1& t1, const T2& t2) {
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t1))));
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t2))));
    }

    template<typename T1, typename T2, typename T3>
    static void get(std::vector<void*>& v, const T1& t1, const T2& t2, const T3& t3) {
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t1))));
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t2))));
        v.push_back(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(t3))));
    }
    // NOLINTEND
};

class BaseExport Safe
{
public:
template<typename Func, typename ... Args>
static void tryInvoke(Func func, Args&& ... args)
{
    try {
        if constexpr (std::is_invocable_v<Func>) {
            func();
        }
        else {
            func(std::forward<decltype(args)>(args)...);
        }
    }
    catch (const Base::Exception& e) {
        Base::Console().Error("Unhandled Base::Exception caught in Safe::tryInvoke.\n"
                              "The error message is: %s\n", e.what());
    }
    catch (const std::exception& e) {
        Base::Console().Error("Unhandled std::exception caught in Safe::tryInvoke.\n"
                              "The error message is: %s\n", e.what());
    }
    catch (...) {
        Base::Console().Error("Unhandled unknown exception caught in Safe::tryInvoke.\n");
    }
}

//connect to a function pointer  (not a member)
template <typename Func1, typename Func2>
static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0, QMetaObject::Connection>::type
        connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
    auto safeSlotWrapper = [slot](auto&&... args) {
        tryInvoke(slot, std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, sender, safeSlotWrapper, Qt::DirectConnection);
}

//connect to a function pointer  (not a member)
template <typename Func1, typename Func2>
static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0 &&
                                      !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
        connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, const QObject *context, Func2 slot,
                Qt::ConnectionType type = Qt::AutoConnection)
{
    auto safeSlotWrapper = [slot](auto&&... args) {
        tryInvoke(slot, std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, context, safeSlotWrapper, type);
}

//connect to a functor
template <typename Func1, typename Func2>
static inline typename std::enable_if<QtPrivate::FunctionPointer<Func2>::ArgumentCount == -1, QMetaObject::Connection>::type
        connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
    auto safeSlotWrapper = [slot](auto&&... args) {
        tryInvoke(slot, std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, sender, safeSlotWrapper);
}

//connect to a functor, with a "context" object defining in which event loop is going to be executed
template <typename Func1, typename Func2>
static inline typename std::enable_if<QtPrivate::FunctionPointer<Func2>::ArgumentCount == -1, QMetaObject::Connection>::type
        connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, const QObject *context, Func2 slot,
                Qt::ConnectionType type = Qt::AutoConnection)
{
    auto safeSlotWrapper = [slot](auto&&... args) {
        tryInvoke(slot, std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, context, safeSlotWrapper, type);
}

//Connect a signal to a pointer to qobject member function
template <typename Func1, typename Func2>
static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                 const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                                 Qt::ConnectionType type = Qt::AutoConnection)
{
    using SignalType = QtPrivate::FunctionPointer<Func1>;
    using SlotType = QtPrivate::FunctionPointer<Func2>;

    auto safeSlotWrapper = [receiver, slot](auto&&... args) {
        try {
            QtPrivate::QSlotObject<Func2,
                    typename QtPrivate::List_Left<
                        typename SignalType::Arguments,
                        SlotType::ArgumentCount>::Value,
                    typename SignalType::ReturnType> slotobj(slot);
            std::vector<void*> arg;
            arg.push_back(nullptr);
            ParseArgs::get(arg, std::forward<decltype(args)>(args)...);
            slotobj.call(const_cast<typename QtPrivate::FunctionPointer<Func2>::Object *>(receiver), arg.data());
        }
        catch (const Base::Exception& e) {
            Base::Console().Error("Unhandled Base::Exception caught in Safe::connect.\n"
                                  "The error message is: %s\n", e.what());
        }
        catch (const std::exception& e) {
            Base::Console().Error("Unhandled std::exception caught in Safe::connect.\n"
                                  "The error message is: %s\n", e.what());
        }
        catch (...) {
            Base::Console().Error("Unhandled unknown exception caught in Safe::connect.\n");
        }
    };

    return QObject::connect(sender, signal, receiver, safeSlotWrapper, type);
}
};

#endif // BASE_SAFE_H
// clang-format on
