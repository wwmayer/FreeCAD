#include <gtest/gtest.h>
#include <cstdio>
#include <memory>
#include <zipios++/zipfile.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
TEST(ZipFile, TestValidity)
{
    zipios::ZipFile zf;
    EXPECT_EQ(zf.isValid(), false);
    EXPECT_THROW(zf.entries(), zipios::InvalidStateException);
    EXPECT_THROW(zf.getEntry("inexistant", zipios::FileCollection::MatchPath::MATCH),
                 zipios::InvalidStateException);
    EXPECT_THROW(zf.getEntry("inexistant", zipios::FileCollection::MatchPath::IGNORE),
                 zipios::InvalidStateException);
    EXPECT_THROW(zf.getInputStream("inexistant", zipios::FileCollection::MatchPath::MATCH),
                 zipios::InvalidStateException);
    EXPECT_THROW(zf.getInputStream("inexistant", zipios::FileCollection::MatchPath::IGNORE),
                 zipios::InvalidStateException);
    EXPECT_THROW(zf.getName(), zipios::InvalidStateException);
    EXPECT_THROW(zf.size(), zipios::InvalidStateException);
    zf.close();
    EXPECT_EQ(zf.isValid(), false);
}

TEST(ZipFile, TestNonExisting)
{
    zipios::ZipFile zf("this/file/does/not/exist");
    EXPECT_EQ(zf.isValid(), false);
}

TEST(ZipFile, TestExisting)
{
    std::string zip(DATADIR);
    zip.append("/tests/test.zip");

    zipios::ZipFile zf(zip);
    EXPECT_TRUE(zf.isValid());
    EXPECT_EQ(zf.size(), 4);

    auto f1 = zf.getEntry("file1.txt");
    EXPECT_TRUE(f1);

    auto f2 = zf.getEntry("file2.txt");
    EXPECT_TRUE(f2);

    auto f3 = zf.getEntry("file3.txt");
    EXPECT_TRUE(f3);

    auto f4 = zf.getEntry("testfile.bin");
    EXPECT_TRUE(f4);

    auto f5 = zf.getEntry("testfile.txt");
    EXPECT_FALSE(f5);
}

class ZipFileTest: public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::ofstream os("empty.zip", std::ios::out | std::ios::binary);
        os << static_cast<char>(0x50);
        os << static_cast<char>(0x4B);
        os << static_cast<char>(0x05);
        os << static_cast<char>(0x06);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
        os << static_cast<char>(0x00);
    }
    void TearDown() override
    {
        // delete empty.zip
        std::remove("empty.zip");
    }
};

TEST_F(ZipFileTest, TestValid)
{
    zipios::ZipFile zf("empty.zip");
    EXPECT_EQ(zf.isValid(), true);
    EXPECT_EQ(zf.entries().empty(), true);
    EXPECT_EQ(zf.getName(), "empty.zip");
    EXPECT_EQ(zf.size(), 0);
    zf.close();
    EXPECT_EQ(zf.isValid(), false);
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
