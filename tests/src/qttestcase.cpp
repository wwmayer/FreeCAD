#include <QTest>

// add necessary includes here

class testCase : public QObject
{
    Q_OBJECT

public:
    testCase();
    ~testCase();

    bool myCondition()
    {
        return true;
    }

private slots:
    void test_case1();
    void initTestCase()
    {
        qDebug("Called before everything else.");
    }
    void cleanupTestCase()
    {
        qDebug("Called after myFirstTest and mySecondTest.");
    }
};

testCase::testCase()
{

}

testCase::~testCase()
{

}

void testCase::test_case1()
{
    QVERIFY(true); // check that a condition is satisfied
    QCOMPARE(1, 1); // compare two values

    QVERIFY(myCondition());
    QVERIFY(1 != 2);
}

QTEST_MAIN(testCase)

#include "qttestcase.moc"
