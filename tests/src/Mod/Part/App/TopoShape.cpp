// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include "PartTestHelpers.h"
#include <Mod/Part/App/TopoShape.h>
#include "src/App/InitApplication.h"


class TopoShapeTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }

    void SetUp() override
    {
        Base::Interpreter().runString("import Part");
        _docName = App::GetApplication().getUniqueDocumentName("test");
        App::GetApplication().newDocument(_docName.c_str(), "testUser");
        _hasher = Base::Reference<App::StringHasher>(new App::StringHasher);
        ASSERT_EQ(_hasher.getRefCount(), 1);
    }

    void TearDown() override
    {
        App::GetApplication().closeDocument(_docName.c_str());
    }


private:
    std::string _docName;
    Data::ElementIDRefs _sid;
    App::StringHasherRef _hasher;
};

// clang-format off
TEST_F(TopoShapeTest, TestElementTypeFace1)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex("Face1"),
              std::make_pair(std::string("Face"), 1UL));
}

TEST_F(TopoShapeTest, TestElementTypeEdge12)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex("Edge12"),
              std::make_pair(std::string("Edge"), 12UL));
}

TEST_F(TopoShapeTest, TestElementTypeVertex3)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex("Vertex3"),
              std::make_pair(std::string("Vertex"), 3UL));
}

TEST_F(TopoShapeTest, TestElementTypeFacer)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex("Facer"),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestElementTypeVertex)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex("Vertex"),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestElementTypeEmpty)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex(""),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestElementTypeNull)
{
    EXPECT_EQ(Part::TopoShape::getElementTypeAndIndex(nullptr),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestTypeFace1)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex("Face1"),
              std::make_pair(std::string("Face"), 1UL));
}

TEST_F(TopoShapeTest, TestTypeEdge12)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex("Edge12"),
              std::make_pair(std::string("Edge"), 12UL));
}

TEST_F(TopoShapeTest, TestTypeVertex3)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex("Vertex3"),
              std::make_pair(std::string("Vertex"), 3UL));
}

TEST_F(TopoShapeTest, TestTypeFacer)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex("Facer"),
              std::make_pair(std::string("Facer"), 0UL));
}

TEST_F(TopoShapeTest, TestTypeVertex)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex("Vertex"),
              std::make_pair(std::string("Vertex"), 0UL));
}

TEST_F(TopoShapeTest, TestTypeEmpty)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex(""),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestTypeNull)
{
    EXPECT_EQ(Part::TopoShape::getTypeAndIndex(nullptr),
              std::make_pair(std::string(), 0UL));
}

TEST_F(TopoShapeTest, TestGetSubshape)
{
    // Arrange
    auto [cube1, cube2] = PartTestHelpers::CreateTwoTopoShapeCubes();
    // Act
    auto face = cube1.getSubShape("Face2");
    auto vertex = cube2.getSubShape(TopAbs_VERTEX,2);
    auto silentFail = cube1.getSubShape("NotThere", true);
    // Assert
    EXPECT_EQ(face.ShapeType(), TopAbs_FACE);
    EXPECT_EQ(vertex.ShapeType(), TopAbs_VERTEX);
    EXPECT_TRUE(silentFail.IsNull());
    EXPECT_THROW(cube1.getSubShape("Face7"), Base::IndexError);          // Out of range
    EXPECT_THROW(cube1.getSubShape("WOOHOO", false), Base::ValueError);  // Invalid
}

class TopoShapeCopy: public ::testing::Test
{
protected:
    Part::TopoShape MakeEdge(gp_Pnt p1, gp_Pnt p2, gp_Vec vec)
    {
        BRepBuilderAPI_MakeVertex mkVertex1(p1);
        BRepBuilderAPI_MakeVertex mkVertex2(p2);
        TopoDS_Vertex v1 = mkVertex1.Vertex();
        TopoDS_Vertex v2 = mkVertex2.Vertex();

        gp_Trsf trsf;
        trsf.SetTranslation(vec);
        TopLoc_Location loc(trsf);
        v1.Location(loc);
        v2.Location(loc);

        // create an edge from two translated vertexes
        BRepBuilderAPI_MakeEdge mkEdge(v1, v2);
        return {mkEdge.Edge()};
    }
    Part::TopoShape MakeCopyOfElement(const Part::TopoShape shape, const char* element)
    {
        Part::TopoShape sub = shape.getSubShape(element, true);
        App::StringHasherRef hasher;
        return Part::TopoShape(0, hasher).makeElementCopy(sub, nullptr, true, false);
    }
};

TEST_F(TopoShapeCopy, TestVertex1Copy)
{
    Part::TopoShape edge(MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(5, 0, 0), gp_Vec(1, 0, 0)));
    Part::TopoShape copy = MakeCopyOfElement(edge, "Vertex1");

    const TopoDS_Vertex& v = TopoDS::Vertex(copy.getShape());
    gp_Pnt p = BRep_Tool::Pnt(v);
    EXPECT_DOUBLE_EQ(p.X(), 1.0);
    EXPECT_DOUBLE_EQ(p.Y(), 0.0);
    EXPECT_DOUBLE_EQ(p.Z(), 0.0);
}

TEST_F(TopoShapeCopy, TestVertex2Copy)
{
    Part::TopoShape edge(MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(5, 0, 0), gp_Vec(1, 0, 0)));
    Part::TopoShape copy = MakeCopyOfElement(edge, "Vertex2");

    const TopoDS_Vertex& v = TopoDS::Vertex(copy.getShape());
    gp_Pnt p = BRep_Tool::Pnt(v);
    EXPECT_DOUBLE_EQ(p.X(), 6.0);
    EXPECT_DOUBLE_EQ(p.Y(), 0.0);
    EXPECT_DOUBLE_EQ(p.Z(), 0.0);
}

// clang-format on
