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

#ifndef IMPORT_COLORTRAITS_H
#define IMPORT_COLORTRAITS_H

#include <App/Color.h>
#include <Quantity_ColorRGBA.hxx>
#include <Standard_Version.hxx>

// NOLINTBEGIN(readability-magic-numbers)
// clang-format off
namespace App
{
// Specialization for Quantity_Color
template<>
struct color_traits<Quantity_Color>
{
    using color_type = Quantity_Color;
    color_traits() = default;
    explicit color_traits(const color_type& ct)
        : ct(ct)
    {}
    float redF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        return static_cast<float>(red);
    }
    float greenF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        return static_cast<float>(green);
    }
    float blueF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        return static_cast<float>(blue);
    }
    float alphaF() const
    {
        return 1.0F;
    }
    void setRedF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(value, green, blue, type);
    }
    void setGreenF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(red, value, blue, type);
    }
    void setBlueF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(red, green, value, type);
    }
    void setAlphaF(float alpha)
    {
        (void)alpha;
    }
    int red() const
    {
        return int(std::lround(redF() * 255.0));
    }
    int green() const
    {
        return int(std::lround(greenF() * 255.0));
    }
    int blue() const
    {
        return int(std::lround(blueF() * 255.0));
    }
    int alpha() const
    {
        return 255;
    }
    void setRed(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(static_cast<double>(value) / 255.0, green, blue, type);
    }
    void setGreen(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(red, static_cast<double>(value) / 255.0, blue, type);
    }
    void setBlue(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.Values(red, green, blue, type);
        ct.SetValues(red, green, static_cast<double>(value) / 255.0, type);
    }
    void setAlpha(int alpha)
    {
        (void)alpha;
    }
    static color_type makeColor(int red, int green, int blue, int alpha = 255)
    {
        (void)alpha;
        return color_type{static_cast<double>(red) / 255.0,
                          static_cast<double>(green) / 255.0,
                          static_cast<double>(blue) / 255.0,
                          type};
    }

private:
    color_type ct;

#if OCC_VERSION_HEX >= 0x070500
    // See https://dev.opencascade.org/content/occt-3d-viewer-becomes-srgb-aware
    static const Quantity_TypeOfColor type {Quantity_TOC_sRGB};
#else
    static const Quantity_TypeOfColor type {Quantity_TOC_RGB};
#endif
};

// Specialization for Quantity_ColorRGBA
template<>
struct color_traits<Quantity_ColorRGBA>
{
    using color_type = Quantity_ColorRGBA;
    color_traits() = default;
    explicit color_traits(const color_type& ct)
        : ct(ct)
    {}
    float redF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.GetRGB().Values(red, green, blue, type);
        return static_cast<float>(red);
    }
    float greenF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.GetRGB().Values(red, green, blue, type);
        return static_cast<float>(green);
    }
    float blueF() const
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        ct.GetRGB().Values(red, green, blue, type);
        return static_cast<float>(blue);
    }
    float alphaF() const
    {
        return ct.Alpha();
    }
    void setRedF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(value, green, blue, type);
        ct.SetRGB(rgb);
    }
    void setGreenF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(red, value, blue, type);
        ct.SetRGB(rgb);
    }
    void setBlueF(float value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(red, green, value, type);
        ct.SetRGB(rgb);
    }
    void setAlphaF(float alpha)
    {
        ct.SetAlpha(alpha);
    }
    int red() const
    {
        return int(std::lround(redF() * 255.0F));
    }
    int green() const
    {
        return int(std::lround(greenF() * 255.0F));
    }
    int blue() const
    {
        return int(std::lround(blueF() * 255.0F));
    }
    int alpha() const
    {
        return int(std::lround(ct.Alpha() * 255.0F));
    }
    void setRed(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(static_cast<double>(value) / 255.0, green, blue, type);
        ct.SetRGB(rgb);
    }
    void setGreen(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(red, static_cast<double>(value) / 255.0, blue, type);
        ct.SetRGB(rgb);
    }
    void setBlue(int value)
    {
        Standard_Real red {};
        Standard_Real green {};
        Standard_Real blue {};
        Quantity_Color rgb = ct.GetRGB();
        rgb.Values(red, green, blue, type);
        rgb.SetValues(red, green, static_cast<double>(value) / 255.0, type);
        ct.SetRGB(rgb);
    }
    void setAlpha(int alpha)
    {
        ct.SetAlpha(static_cast<float>(alpha) / 255.0F);
    }
    static color_type makeColor(int red, int green, int blue, int alpha = 255)
    {
        Quantity_Color rgb{static_cast<double>(red) / 255.0,
                           static_cast<double>(green) / 255.0,
                           static_cast<double>(blue) / 255.0,
                           type};
        return color_type{rgb, static_cast<float>(alpha) / 255.0F};
    }

private:
    color_type ct;

#if OCC_VERSION_HEX >= 0x070500
    // See https://dev.opencascade.org/content/occt-3d-viewer-becomes-srgb-aware
    static const Quantity_TypeOfColor type {Quantity_TOC_sRGB};
#else
    static const Quantity_TypeOfColor type {Quantity_TOC_RGB};
#endif
};

}  // namespace App
// clang-format on
// NOLINTEND(readability-magic-numbers)

#endif  // IMPORT_COLORTRAITS_H
