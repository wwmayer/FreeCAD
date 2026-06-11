#include <gtest/gtest.h>
#include <memory>
#include <zipios++/dircoll.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
class DirectoryCollection: public ::testing::Test
{
protected:
    std::string getDirectory() const
    {
        std::string dir(DATADIR);
        dir.append("/../src/3rdParty/zipios++");
        return dir;
    }

    int getNumEntries() const
    {
        return 51; // number of files
    }
};

TEST_F(DirectoryCollection, TestValidity)
{
    zipios::DirectoryCollection collection(getDirectory());
    EXPECT_TRUE(collection.isValid());

    zipios::ConstEntryPointer ent = collection.getEntry("zipios-config.h");
    EXPECT_TRUE(ent);
    std::unique_ptr<std::istream> is(collection.getInputStream(ent));

    EXPECT_EQ(ent->getName(), "zipios-config.h");
    EXPECT_TRUE(is->rdbuf());

    EXPECT_EQ(collection.size(), getNumEntries());

    zipios::ConstEntries entries;
    entries = collection.entries();

    zipios::ConstEntries::iterator it;
    for (it = entries.begin(); it != entries.end(); it++) {
        EXPECT_TRUE((*it)->isValid());
    }

    ent = collection.getEntry("zipios-config.h");
    if (ent) {
        std::unique_ptr<std::istream> is(collection.getInputStream(ent));

        EXPECT_EQ(ent->getName(), "zipios-config.h");
        EXPECT_TRUE(is->rdbuf());
    }

    collection.close();
    EXPECT_FALSE(collection.isValid());
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
