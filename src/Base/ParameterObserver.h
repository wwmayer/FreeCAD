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

#ifndef BASE_PARAMETEROBSERVER_H
#define BASE_PARAMETEROBSERVER_H

#include <Base/Parameter.h>
#include <FCGlobal.h>
#include <variant>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>

namespace Base {

class BaseExport ParameterObserver : public ParameterGrp::ObserverType {
protected:
    using Type = std::variant<bool, long, unsigned long, double, std::string>;
    class Object {

    public:
        template <typename T>
        Object(T&& obj): object(std::make_shared<Model<T>>(std::forward<T>(obj))){}

        void fetch(ParameterGrp::handle handle, const char* key) {
            object->fetch(handle, key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            object->setParameter(handle, key, value);
        }
        Type value() const {
            return object->value();
        }
        Type defaultValue() const {
            return object->defaultValue();
        }
        void setValue(const Type& value) {
            object->setValue(value);
        }

        struct Concept {
            virtual ~Concept() {}
            virtual void fetch(ParameterGrp::handle, const char* key) = 0;
            virtual void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) = 0;
            virtual Type value() const = 0;
            virtual Type defaultValue() const = 0;
            virtual void setValue(const Type&) = 0;
        };

        template< typename T >
        struct Model : Concept {
            Model(const T& t) : object(t) {}
            void fetch(ParameterGrp::handle handle, const char* key) override {
                object.fetch(handle, key);
            }
            void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) override {
                object.setParameter(handle, key, value);
            }
            Type value() const override {
                return object.value();
            }
            Type defaultValue() const override {
                return object.defaultValue();
            }
            void setValue(const Type& v) override {
                object.setValue(v);
            }
        private:
            T object;
        };

        std::shared_ptr<Concept> object;
    };

public:
    struct BaseType {
        Type _value, _default;
        BaseType(Type d) : _value(d), _default(d) {}
        Type value() const {
            return _value;
        }
        Type defaultValue() const {
            return _default;
        }
        void setValue(const Type& v) {
            _value = v;
        }
    };

    struct Bool : BaseType {
        using value_type = bool;
        Bool(value_type d) : BaseType{d} {}
        void fetch(ParameterGrp::handle handle, const char* key) {
            _value = handle->GetBool(key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            handle->SetBool(key, std::get<value_type>(value));
        }
    };

    struct Int : BaseType {
        using value_type = long;
        Int(value_type d) : BaseType{d} {}
        void fetch(ParameterGrp::handle handle, const char* key) {
            _value = handle->GetInt(key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            handle->SetInt(key, std::get<value_type>(value));
        }
    };

    struct Unsigned : BaseType {
        using value_type = unsigned long;
        Unsigned(value_type d) : BaseType{d} {}
        void fetch(ParameterGrp::handle handle, const char* key) {
            _value = handle->GetUnsigned(key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            handle->SetUnsigned(key, std::get<value_type>(value));
        }
    };

    struct Double : BaseType {
        using value_type = double;
        Double(value_type d) : BaseType{d} {}
        void fetch(ParameterGrp::handle handle, const char* key) {
            _value = handle->GetFloat(key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            handle->SetFloat(key, std::get<value_type>(value));
        }
    };

    struct String : BaseType {
        using value_type = std::string;
        String(const value_type& d) : BaseType{d} {}
        void fetch(ParameterGrp::handle handle, const char* key) {
            _value = handle->GetASCII(key);
        }
        void setParameter(ParameterGrp::handle handle, const char* key, const Type& value) {
            handle->SetASCII(key, std::get<value_type>(value).c_str());
        }
    };

private:
    std::unordered_map<const char *, Object> parameters;

protected:
    ParameterGrp::handle handle;
    ParameterObserver();

    void initParameters();
    void OnChange(Base::Subject<const char*> &, const char* sReason);
    void addParameter(const char* key, const Object& value);

    void setBoolean(const char* key, bool value);
    bool getBoolean(const char* key) const;
    bool getDefaultBoolean(const char* key) const;

    void setInt(const char* key, long value);
    long getInt(const char* key) const;
    long getDefaultInt(const char* key) const;

    void setUnsigned(const char* key, unsigned long value);
    unsigned long getUnsigned(const char* key) const;
    unsigned long getDefaultUnsigned(const char* key) const;

    void setFloat(const char* key, double value);
    double getFloat(const char* key) const;
    double getDefaultFloat(const char* key) const;

    void setString(const char* key, const char* value);
    std::string getString(const char* key) const;
    std::string getDefaultString(const char* key) const;

    template <typename T>
    void setValue(const char* key, const T& value) {
        try {
            parameters.at(key).setParameter(handle, key, value);
            parameters.at(key).setValue(value);
        }
        catch (const std::bad_variant_access&) {
            std::stringstream str;
            str << "Wrong cast of parameter " << key << '\n';
            throw Base::TypeError(str.str());
        }
    }
    template <typename T>
    T getValue(const char* key) const {
        try {
            return std::get<T>(parameters.at(key).value());
        }
        catch (const std::bad_variant_access&) {
            std::stringstream str;
            str << "Wrong cast of parameter " << key << '\n';
            throw Base::TypeError(str.str());
        }
    }
    template <typename T>
    T getDefault(const char* key) const {
        try {
            return std::get<T>(parameters.at(key).defaultValue());
        }
        catch (const std::bad_variant_access&) {
            std::stringstream str;
            str << "Wrong cast of parameter " << key << '\n';
            throw Base::TypeError(str.str());
        }
    }
};

template <class T>
struct type_traits { };

template <>
struct type_traits<bool> {
    using value_type = ParameterObserver::Bool;
};

template <>
struct type_traits<long> {
    using value_type = ParameterObserver::Int;
};

template <>
struct type_traits<unsigned long> {
    using value_type = ParameterObserver::Unsigned;
};

template <>
struct type_traits<double> {
    using value_type = ParameterObserver::Double;
};

template <>
struct type_traits<std::string> {
    using value_type = ParameterObserver::String;
};

}

#endif // BASE_PARAMETEROBSERVER_H
