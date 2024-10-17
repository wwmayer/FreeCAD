#include <gtest/gtest.h>
#include <Base/Exception.h>
#include <Base/Safe.h>

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <string>

// NOLINTBEGIN
class TestObject: public QObject
{
    Q_OBJECT

public:
    void invokeSignalVoid()
    {
        Q_EMIT emitSignalVoid();
    }
    void invokeSignalInt()
    {
        Q_EMIT emitSignalInt(5);
    }
    void invokeSignalIntDouble()
    {
        Q_EMIT emitSignalIntDouble(4, 1.7);
    }
    void invokeSignalIntInt()
    {
        Q_EMIT emitSignalIntInt(9, 7);
    }
    void invokeSignalString()
    {
        Q_EMIT emitSignalString(QString("methodSlotString"));
    }
    void invokeAllSignals()
    {
        invokeSignalVoid();
        invokeSignalInt();
        invokeSignalIntDouble();
        invokeSignalIntInt();
        invokeSignalString();
    }
    bool hasQueuedConnection() const
    {
        return calledQueuedConnection;
    }
    void methodQueuedConnection()
    {
        calledQueuedConnection = true;
    }
    void methodSlotVoidRaise()
    {
        throw Base::RuntimeError("methodSlotVoidRaise");
    }
    void methodSlotIntRaise(int i)
    {
        EXPECT_EQ(i, 5);
        throw Base::RuntimeError("methodSlotIntRaise");
    }
    void methodSlotVoid()
    {}
    void methodSlotInt(int i)
    {
        EXPECT_EQ(i, 5);
    }
    void methodSlotOneIntInt(int i)
    {
        EXPECT_EQ(i, 9);
    }
    void methodSlotIntInt(int i, int j)
    {
        EXPECT_EQ(i, 9);
        EXPECT_EQ(j, 7);
    }
    void methodSlotIntDouble(int i, double f)
    {
        EXPECT_EQ(i, 4);
        EXPECT_DOUBLE_EQ(f, 1.7);
    }
    void methodSlotString(const QString& str)
    {
        EXPECT_EQ(str, QString("methodSlotString"));
    }

Q_SIGNALS:
    void emitSignalVoid();
    void emitSignalInt(int);
    void emitSignalIntDouble(int, double);
    void emitSignalIntInt(int, int);
    void emitSignalString(const QString&);

private:
    bool calledQueuedConnection = false;
};

static void functionVoid()
{
    qDebug("Slot func\n");
}

static void functionVoidRaise()
{
    throw Base::RuntimeError("testRaiseNoArg");
}

static void functionInt(int val)
{
    EXPECT_EQ(val, 5);
}

static void functionIntInt(int i, int j)
{
    EXPECT_EQ(i, 9);
    EXPECT_EQ(j, 7);
}

static void functionIntDouble(int i, double d)
{
    EXPECT_EQ(i, 4);
    EXPECT_DOUBLE_EQ(d, 1.7);
}

static void functionIntRaise(int val)
{
    EXPECT_EQ(val, 5);
    throw Base::RuntimeError("testRaise");
}

static void functionString(const QString& str)
{
    EXPECT_EQ(str, QString("methodSlotString"));
}

// ------------------------------------------------------------------------------------------------

TEST(SafeTest, TestQObjectConnect)
{
    auto obj = new TestObject();
    QObject::connect(obj, &TestObject::emitSignalInt, obj, &TestObject::methodSlotInt);
    QObject::connect(obj, &TestObject::emitSignalIntDouble, obj, &TestObject::methodSlotIntDouble);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestBindConnect)
{
    auto obj = new TestObject();
    auto func = std::bind(&TestObject::methodSlotVoidRaise, obj);
    Safe::connect(obj, &TestObject::emitSignalInt, obj, func);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, obj, &TestObject::methodSlotInt);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotOneIntInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalIntInt, obj, &TestObject::methodSlotOneIntInt);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotIntInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalIntInt, obj, &TestObject::methodSlotIntInt);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotIntDouble)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalIntDouble, obj, &TestObject::methodSlotIntDouble);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotVoid)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalVoid, obj, &TestObject::methodSlotVoid);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodSlotVoidRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, obj, &TestObject::methodSlotVoidRaise);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestSlotIntRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, obj, &TestObject::methodSlotIntRaise);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestMethodString)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalString, obj, &TestObject::methodSlotString);
    obj->invokeAllSignals();
    delete obj;
}

// Creating a QCoreApplication causes to fail:
// Expression.tokenizeNum
// ExpressionParserTest.functionPARSEQUANT
// PropertyExpressionEngineTest.executeCrossPropertyReference
TEST(SafeTest, DISABLED_TestMethodQueuedConnection)
{
    int argc = 1;
    std::vector<char*> argv = {"test"};
    QCoreApplication app(argc, argv.data());

    auto obj = new TestObject();
    Safe::connect(obj,
                  &TestObject::emitSignalInt,
                  obj,
                  &TestObject::methodQueuedConnection,
                  Qt::QueuedConnection);
    obj->invokeAllSignals();
    EXPECT_FALSE(obj->hasQueuedConnection());
    app.processEvents();
    EXPECT_TRUE(obj->hasQueuedConnection());
    delete obj;
}

TEST(SafeTest, TestFunctionVoidRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, &functionVoidRaise);
    Safe::connect(obj, &TestObject::emitSignalIntDouble, &functionVoidRaise);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, &functionInt);
    Safe::connect(obj, &TestObject::emitSignalInt, obj, &functionInt, Qt::AutoConnection);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionIntInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalIntInt, &functionIntInt);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionIntDouble)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalIntDouble, &functionIntDouble);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionIntVoid)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, &functionVoid);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionIntVoidRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, &functionVoidRaise);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionIntRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, &functionIntRaise);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestFunctionString)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalString, &functionString);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestAutoConnection)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, obj, &functionVoidRaise, Qt::AutoConnection);
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestLambdaInt)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, [=](int val) {
        EXPECT_EQ(val, 5);
    });
    Safe::connect(obj, &TestObject::emitSignalInt, obj, [=](int val) {
        EXPECT_EQ(val, 5);
    });
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestLambdaVoid)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, [=]() {
        qDebug("Slot func:\n");
    });
    Safe::connect(obj, &TestObject::emitSignalInt, obj, [=]() {
        qDebug("Slot func: \n");
    });
    obj->invokeAllSignals();
    delete obj;
}

TEST(SafeTest, TestLambdaVoidRaise)
{
    auto obj = new TestObject();
    Safe::connect(obj, &TestObject::emitSignalInt, [=]() {
        throw Base::RuntimeError("Slot func:\n");
    });
    Safe::connect(obj, &TestObject::emitSignalInt, obj, [=]() {
        throw Base::RuntimeError("Slot func: \n");
    });
    obj->invokeAllSignals();
    delete obj;
}
// NOLINTEND

#include "Safe.moc"
