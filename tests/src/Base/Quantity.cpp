#include <gtest/gtest.h>
#include <Base/Exception.h>
#include <Base/Quantity.h>
#include <Base/UnitsApi.h>
#include <Base/UnitsSchemaImperial1.h>
#include <QLocale>
#include <boost/core/ignore_unused.hpp>

// NOLINTBEGIN
TEST(BaseQuantity, TestValid)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    Base::Quantity q2 {1.0, Base::Unit::Area};
    q2.setInvalid();

    EXPECT_EQ(q1.isValid(), true);
    EXPECT_EQ(q2.isValid(), false);
}

TEST(BaseQuantity, TestParse)
{
    Base::Quantity q1 = Base::Quantity::parse("1,234 kg");

    EXPECT_EQ(q1, Base::Quantity(1.2340, Base::Unit::Mass));
    EXPECT_THROW(boost::ignore_unused(Base::Quantity::parse("1,234,500.12 kg")), Base::ParserError);
}

TEST(BaseQuantity, TestDim)
{
    Base::Quantity q1 {0, Base::Unit::Area};

    EXPECT_EQ(q1.isQuantity(), true);
}

TEST(BaseQuantity, TestNoDim)
{
    Base::Quantity q1 {};

    EXPECT_EQ(q1.pow(2), Base::Quantity {0});
    EXPECT_EQ(q1.isDimensionless(), true);
}

TEST(BaseQuantity, TestPowEQ1)
{
    Base::Quantity q1 {2, Base::Unit::Area};
    EXPECT_EQ(q1.pow(1), Base::Quantity(2, Base::Unit::Area));
}

TEST(BaseQuantity, TestPowEQ0)
{
    Base::Quantity q1 {2, Base::Unit::Area};
    EXPECT_EQ(q1.pow(0), Base::Quantity {1});
}

TEST(BaseQuantity, TestPowGT1)
{
    Base::Quantity q1 {2, Base::Unit::Length};
    EXPECT_EQ(q1.pow(2), Base::Quantity(4, Base::Unit::Area));
}

TEST(BaseQuantity, TestPowLT1)
{
    Base::Quantity q1 {8, Base::Unit::Volume};
    EXPECT_EQ(q1.pow(1.0 / 3.0), Base::Quantity(2, Base::Unit::Length));
}

TEST(BaseQuantity, TestPow3DIV2)
{
    Base::Quantity unit {8, Base::Unit::Volume};
    EXPECT_THROW(unit.pow(3.0 / 2.0), Base::UnitsMismatchError);
}

TEST(BaseQuantity, TestString)
{
    Base::Quantity q1 {2, "kg*m/s^2"};
    EXPECT_EQ(q1.getUnit(), Base::Unit::Force);

    Base::Quantity q2 {2, "kg*m^2/s^2"};
    EXPECT_EQ(q2.getUnit(), Base::Unit::Work);
}

TEST(BaseQuantity, TestCopy)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};

    EXPECT_EQ(Base::Quantity {q1}, q1);
}

TEST(BaseQuantity, TestEqual)
{
    Base::Quantity q1 {1.0, Base::Unit::Force};
    Base::Quantity q2 {1.0, "kg*mm/s^2"};

    EXPECT_EQ(q1 == q1, true);
    EXPECT_EQ(q1 == q2, true);
}

TEST(BaseQuantity, TestNotEqual)
{
    Base::Quantity q1 {1.0, Base::Unit::Force};
    Base::Quantity q2 {2.0, "kg*m/s^2"};
    Base::Quantity q3 {1.0, Base::Unit::Work};

    EXPECT_EQ(q1 != q2, true);
    EXPECT_EQ(q1 != q3, true);
}

TEST(BaseQuantity, TestLessOrGreater)
{
    Base::Quantity q1 {1.0, Base::Unit::Force};
    Base::Quantity q2 {2.0, "kg*m/s^2"};
    Base::Quantity q3 {2.0, Base::Unit::Work};

    EXPECT_EQ(q1 < q2, true);
    EXPECT_EQ(q1 > q2, false);
    EXPECT_EQ(q1 <= q1, true);
    EXPECT_EQ(q1 >= q1, true);
    EXPECT_THROW(boost::ignore_unused(q1 < q3), Base::UnitsMismatchError);
    EXPECT_THROW(boost::ignore_unused(q1 > q3), Base::UnitsMismatchError);
    EXPECT_THROW(boost::ignore_unused(q1 <= q3), Base::UnitsMismatchError);
    EXPECT_THROW(boost::ignore_unused(q1 >= q3), Base::UnitsMismatchError);
}

TEST(BaseQuantity, TestAdd)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    Base::Quantity q2 {1.0, Base::Unit::Area};
    EXPECT_THROW(q1 + q2, Base::UnitsMismatchError);
    EXPECT_THROW(q1 += q2, Base::UnitsMismatchError);
    EXPECT_EQ(q1 + q1, Base::Quantity(2, Base::Unit::Length));
    EXPECT_EQ(q1 += q1, Base::Quantity(2, Base::Unit::Length));
}

TEST(BaseQuantity, TestSub)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    Base::Quantity q2 {1.0, Base::Unit::Area};
    EXPECT_THROW(q1 - q2, Base::UnitsMismatchError);
    EXPECT_THROW(q1 -= q2, Base::UnitsMismatchError);
    EXPECT_EQ(q1 - q1, Base::Quantity(0, Base::Unit::Length));
    EXPECT_EQ(q1 -= q1, Base::Quantity(0, Base::Unit::Length));
}

TEST(BaseQuantity, TestNeg)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    EXPECT_EQ(-q1, Base::Quantity(-1.0, Base::Unit::Length));
}

TEST(BaseQuantity, TestMult)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    Base::Quantity q2 {1.0, Base::Unit::Area};
    EXPECT_EQ(q1 * q2, Base::Quantity(1.0, Base::Unit::Volume));
    EXPECT_EQ(q1 * 2.0, Base::Quantity(2.0, Base::Unit::Length));
}

TEST(BaseQuantity, TestDiv)
{
    Base::Quantity q1 {1.0, Base::Unit::Length};
    Base::Quantity q2 {1.0, Base::Unit::Area};
    EXPECT_EQ(q1 / q2, Base::Quantity(1.0, Base::Unit::InverseLength));
    EXPECT_EQ(q1 / 2.0, Base::Quantity(0.5, Base::Unit::Length));
}

TEST(BaseQuantity, TestPow)
{
    Base::Quantity q1 {2.0, Base::Unit::Length};
    Base::Quantity q2 {2.0, Base::Unit::Area};
    Base::Quantity q3 {0.0};
    EXPECT_EQ(q1.pow(q3), Base::Quantity {1});
    EXPECT_EQ(q1.pow(2.0), Base::Quantity(4, Base::Unit::Area));
    EXPECT_THROW(q1.pow(q2), Base::UnitsMismatchError);
}

class Quantity: public ::testing::Test
{
protected:
    void SetUp() override
    {
        QLocale loc(QLocale::C);
        QLocale::setDefault(loc);
    }
    void TearDown() override
    {}
    Base::Quantity getLength(double value) const
    {
        return getQuantity(value, Base::Unit::Length);
    }
    Base::Quantity getArea(double value) const
    {
        return getQuantity(value, Base::Unit::Area);
    }
    Base::Quantity getVolume(double value) const
    {
        return getQuantity(value, Base::Unit::Volume);
    }
    Base::Quantity getPower(double value) const
    {
        return getQuantity(value, Base::Unit::Power);
    }
    Base::Quantity getElectricPotential(double value) const
    {
        return getQuantity(value, Base::Unit::ElectricPotential);
    }
    Base::Quantity getHeatFlux(double value) const
    {
        return getQuantity(value, Base::Unit::HeatFlux);
    }
    Base::Quantity getVelocity(double value) const
    {
        return getQuantity(value, Base::Unit::Velocity);
    }
    Base::Quantity getQuantity(double value, Base::Unit unit) const
    {
        Base::Quantity quantity {value, unit};
        Base::QuantityFormat format = quantity.getFormat();
        format.precision = 6;
        quantity.setFormat(format);
        return quantity;
    }
};

TEST_F(Quantity, TestSchemeImperialTwo)
{
    Base::Quantity quantity {1.0, Base::Unit::Length};

    double factor {};
    std::string unitString;
    auto scheme = Base::UnitsApi::createSchema(Base::UnitSystem::ImperialDecimal);
    std::string result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.04 in");
}

TEST_F(Quantity, TestSchemeImperialOne)
{
    Base::Quantity quantity {1.0, Base::Unit::Length};

    Base::QuantityFormat format = quantity.getFormat();
    format.precision = 1;
    quantity.setFormat(format);

    double factor {};
    std::string unitString;
    auto scheme = Base::UnitsApi::createSchema(Base::UnitSystem::ImperialDecimal);
    std::string result = scheme->schemaTranslate(quantity, factor, unitString);

    EXPECT_EQ(result, "0.0 in");
}

TEST_F(Quantity, TestSchemeMeterDecimal)
{
    double factor {};
    std::string unitString;
    std::string result;
    Base::Quantity quantity;

    auto scheme = Base::UnitsApi::createSchema(Base::UnitSystem::MeterDecimal);

    quantity = getLength(1.0);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.001000 m");

    quantity = getArea(15);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.000015 m^2");

    quantity = getVolume(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.123456 m^3");

    quantity = getPower(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 W");

    quantity = getElectricPotential(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 V");

    quantity = getHeatFlux(123.456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 W/m^2");

    quantity = getVelocity(123.456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.123456 m/s");
}

TEST_F(Quantity, TestSchemeMKS)
{
    double factor {};
    std::string unitString;
    std::string result;
    Base::Quantity quantity;

    auto scheme = Base::UnitsApi::createSchema(Base::UnitSystem::SI2);

    quantity = getLength(1.0);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "1.000000 mm");

    quantity = getArea(15);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "15.000000 mm^2");

    quantity = getVolume(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 l");

    quantity = getPower(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 W");

    quantity = getElectricPotential(123456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 V");

    quantity = getHeatFlux(123.456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "123.456000 W/m^2");

    quantity = getVelocity(123.456000);
    result = scheme->schemaTranslate(quantity, factor, unitString);
    EXPECT_EQ(result, "0.123456 m/s");
}

TEST_F(Quantity, TestSchemeGeneric)
{
    std::array<Base::Unit, 55> units = {{
        Base::Unit::Length,
        Base::Unit::Mass,
        Base::Unit::Area,
        Base::Unit::Density,
        Base::Unit::Volume,
        Base::Unit::TimeSpan,
        Base::Unit::Frequency,
        Base::Unit::Velocity,
        Base::Unit::Acceleration,
        Base::Unit::Temperature,
        Base::Unit::CurrentDensity,
        Base::Unit::ElectricCurrent,
        Base::Unit::ElectricPotential,
        Base::Unit::ElectricCharge,
        Base::Unit::SurfaceChargeDensity,
        Base::Unit::MagneticFieldStrength,
        Base::Unit::MagneticFlux,
        Base::Unit::MagneticFluxDensity,
        Base::Unit::Magnetization,
        Base::Unit::ElectricalCapacitance,
        Base::Unit::ElectricalInductance,
        Base::Unit::ElectricalConductance,
        Base::Unit::ElectricalResistance,
        Base::Unit::ElectricalConductivity,
        Base::Unit::ElectromagneticPotential,
        Base::Unit::AmountOfSubstance,
        Base::Unit::LuminousIntensity,
        Base::Unit::CompressiveStrength,
        Base::Unit::Pressure,
        Base::Unit::ShearModulus,
        Base::Unit::Stress,
        Base::Unit::UltimateTensileStrength,
        Base::Unit::YieldStrength,
        Base::Unit::YoungsModulus,
        Base::Unit::Stiffness,
        Base::Unit::StiffnessDensity,
        Base::Unit::Force,
        Base::Unit::Work,
        Base::Unit::Power,
        Base::Unit::Moment,
        Base::Unit::SpecificEnergy,
        Base::Unit::ThermalConductivity,
        Base::Unit::ThermalExpansionCoefficient,
        Base::Unit::VolumetricThermalExpansionCoefficient,
        Base::Unit::SpecificHeat,
        Base::Unit::ThermalTransferCoefficient,
        Base::Unit::HeatFlux,
        Base::Unit::DynamicViscosity,
        Base::Unit::KinematicViscosity,
        Base::Unit::VacuumPermittivity,
        Base::Unit::VolumeFlowRate,
        Base::Unit::DissipationRate,
        Base::Unit::InverseLength,
        Base::Unit::InverseArea,
        Base::Unit::InverseVolume,
    }};

    std::array values = {0.01, 0.1, 1.0, 10.0, 100.0};

    double factor {};
    std::string unitString;
    Base::UnitsSchemaPtr scheme;

    Base::UnitsApi::setDecimals(16);

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::SI1);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::SI2);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::Imperial1);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_NEAR(q2.getValue(), value, 0.001);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::ImperialDecimal);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_NEAR(q2.getValue(), value, 0.001);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::Centimeters);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::MmMin);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::ImperialCivil);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_NEAR(q2.getValue(), value, 0.001);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::FemMilliMeterNewton);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }

    scheme = Base::UnitsApi::createSchema(Base::UnitSystem::MeterDecimal);
    for (auto unit : units) {
        for (double value : values) {
            Base::Quantity q1 {value, unit};
            std::string result = scheme->schemaTranslate(q1, factor, unitString);
            Base::Quantity q2 = Base::Quantity::parse(result);
            EXPECT_DOUBLE_EQ(q2.getValue(), value);
        }
    }
}

TEST_F(Quantity, TestSafeUserString)
{
    Base::UnitsApi::setSchema(Base::UnitSystem::ImperialDecimal);

    Base::Quantity quantity {1.0, Base::Unit::Length};
    Base::QuantityFormat format = quantity.getFormat();
    format.precision = 1;
    quantity.setFormat(format);

    std::string result = quantity.getSafeUserString();

    EXPECT_EQ(result, "1 mm");

    Base::UnitsApi::setSchema(Base::UnitSystem::Imperial1);

    quantity = Base::Quantity {304.8, Base::Unit::Length};
    quantity.setFormat(format);

    result = quantity.getSafeUserString();

    EXPECT_EQ(result, "1.0 \\'");

    quantity = Base::Quantity {25.4, Base::Unit::Length};
    quantity.setFormat(format);

    result = quantity.getSafeUserString();

    EXPECT_EQ(result, "1.0 \\\"");
}
// NOLINTEND
