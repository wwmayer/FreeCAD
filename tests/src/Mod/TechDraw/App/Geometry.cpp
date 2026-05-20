// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>

#include <Mod/TechDraw/App/Geometry.h>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <TopoDS.hxx>

class GeometryTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }

    TopoDS_Edge makeLine() const
    {
        BRepBuilderAPI_MakeEdge aMakeEdge(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(10.0, 10.0, 10.0));
        BRepBuilderAPI_NurbsConvert mkNurbs(aMakeEdge.Edge());
        return TopoDS::Edge(mkNurbs.Shape());
    }
    TopoDS_Edge makeCircle() const
    {
        gp_Circ circle;
        circle.SetRadius(5.0);

        Handle(Geom_Circle) hCircle = new Geom_Circle (circle);
        BRepBuilderAPI_MakeEdge aMakeEdge(hCircle);
        BRepBuilderAPI_NurbsConvert mkNurbs(aMakeEdge.Edge());
        return TopoDS::Edge(mkNurbs.Shape());
    }
};

TEST_F(GeometryTest, isLine)  // NOLINT
{
    EXPECT_TRUE(TechDraw::GeometryUtils::isLine(makeLine()));
    EXPECT_FALSE(TechDraw::GeometryUtils::isLine(makeCircle()));
}
